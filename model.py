
import tensorflow as tf
from tensorflow import keras
import pandas as pd
from sklearn.utils import shuffle
from sklearn.model_selection import train_test_split
import numpy as np
import seaborn as sbn


# In[2]:


#assign normal condition data into a new dataframe
data_nor = pd.read_csv("Monit_Normal.csv") 


# In[3]:


#assign faulty condition data into another dataframe
data_fault = pd.read_csv("Monit_Faulty.csv")


# In[4]:


#label the normal condition data as 0 and fault condition as 1 
data_nor["Output"] = 0
data_fault["Output"] = 1


# In[5]:


#concat both the dataframe 
Data_com = pd.concat([data_nor, data_fault], ignore_index=True)


# In[6]:


Data_com = Data_com.drop(columns=['Unnamed: 4'])


# In[7]:


#to avoid the overfitting, here we shuffle the data in random order
Data_com = shuffle(Data_com).reset_index(drop=True)


# In[8]:


#define and split parameters for the AI to train and test
x = Data_com[['ax','ay','az']].values
y = Data_com['Output'].values

x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.3, random_state = 42)


# In[19]:


#develop a multi layer neural network model

model = tf.keras.Sequential([
    keras.layers.Input(shape=(3,)),
    keras.layers.Dense(16, activation='relu'),
    keras.layers.Dense(8, activation='relu'),
    keras.layers.Dense(1, activation='sigmoid')
])

model.compile(
    optimizer = 'adam',
    loss = 'binary_crossentropy',
    metrics = ['accuracy']
)

model.fit(x_train, y_train, epochs = 20)


# In[20]:


predicted = model(x_test)
predicted = predicted.numpy()
predicted = (predicted >= 0.5).astype(int)


# In[33]:


#To identify the precision and recall of the model
cm=tf.math.confusion_matrix(predicted, y_test)
sbn.heatmap(cm, annot=True, fmt='d')


# In[50]:


#conversion of TFmodel into TFlite, for making it microcontroller compatible
converter = tf.lite.TFLiteConverter.from_keras_model(model)
lite_model = converter.convert()


# In[54]:


with open('Lite_model.tflite', 'wb') as f:
    f.write(lite_model)


# In[55]:


#convert the TFlite into hex file, for microcontroller understanding
def generate_c_header(bytes_data, var_name):
    hex_array = [f'0x{b:02x}' for b in bytes_data]
    c_str = f'const unsigned char {var_name}[] __attribute__((aligned(4))) = {{\n  '
    c_str += ',\n  '.join([', '.join(hex_array[i:i+12]) for i in range(0, len(hex_array), 12)])
    c_str += f'\n}};\n\nconst unsigned int {var_name}_len = {len(bytes_data)};'
    return c_str

with open ('Converted_model', 'w') as f:
    f.write(generate_c_header(Lite_quant_model, "Converted_model"))

