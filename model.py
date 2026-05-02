#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import numpy as np
import pandas as pd
import tensorflow as tf
import os
from tensorflow import keras
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle
import seaborn as sbn
import matplotlib.pyplot as plt
get_ipython().run_line_magic('matplotlib', 'inline')


# In[ ]:


data = pd.read_csv("Normal.csv")[['ax','ay','az']]
data_2 = pd.read_csv('Faulty.csv')[['ax','ay','az']]
test = pd.read_csv('test.csv')[['ax', 'ay', 'az']]


# In[ ]:


data_set = pd.read_csv('')


# In[ ]:


test.head(-1)


# In[ ]:


data_2.head()


# In[ ]:


data_2['output']=1
data['output']=0


# In[ ]:


data.describe()


# In[ ]:


data_combined = pd.concat([data, data_2], ignore_index=True)


# In[ ]:


data_combined = shuffle(data_combined).reset_index(drop=True)


# In[ ]:


plt.plot(data_combined.index, data_combined.ay)
plt.show()


# In[ ]:


normal = data_combined.loc[data_combined['output'] == 0]
anomaly = data_combined.loc[data_combined['output'] == 1]

plt.plot(normal.index, normal['ay'], color='green', label='Normal')
plt.plot(anomaly.index, anomaly['ay'], color='blue', label='Anomaly')
plt.xlim(0,10)
plt.show()


# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:


import matplotlib.pyplot as plt

fig, ay1 = plt.subplots()

# First X-axis (Bottom)
ay1.plot(data_combined.index, data_combined['ax'], color='blue')
ay1.set_xlabel('X_accelo', color='blue')
ay1.set_ylabel('Time interval')
plt.xlim(10,100)
# Second X-axis (Top)
ay2 = ay1.twiny() 
ay2.plot(data_combined.index, data_combined['output'], color='red')
ay2.set_xlabel('Machine status', color='red')
plt.xlim(10,100)
ay3 = ay1.twiny()
ay3.plot(data_combined.index, data_combined.ay, color='green', label='y axis')
ay3.set_xlabel('Y_accelo', color='red')
plt.xlim(10,100)
ay4 = ay1.twiny()
ay4.plot(data_combined.index, data_combined.az, color='purple')
ay4.set_xlabel('Z_accelo', color='red')

plt.xlim(10,100)
plt.show()


# In[ ]:


x = data_combined[['ax', 'ay', 'az']].values 
y = data_combined['output'].values

x_train, x_test, y_train, y_test = train_test_split(x, y, test_size = 0.3, random_state=42)


# In[ ]:


x_train.shape


# In[ ]:


y_test.shape


# In[ ]:


model = tf.keras.Sequential([
    keras.layers.Input(shape=(3,)),
    keras.layers.Dense(64, activation='relu'),
    keras.layers.Dense(1, activation='sigmoid'),
])

model.compile(
    optimizer='adam',
    loss='binary_crossentropy',
    metrics = ['accuracy']
)

model.fit(x_train, y_train, epochs=10)


# In[ ]:


model.evaluate(x_test, y_test)


# In[ ]:


model.summary()


# In[ ]:





# In[ ]:


model_2 = tf_keras.layers.sequential(
    tf_keras.layers.input(shape=(3,)),
    tf_keras.layers.Dense(32, activation='relu'),
    tf_keras.layers.Dense(1, activation='sigmoid')
)

model.compile(
    optimiser = 'adam',
    loss = 'binary_crossentropy',
    metrics = ['accuracy']
)

model.fit(x_train, y_train, epochs = 10)


# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:


predicted = model.predict(x_test)
predicted = predicted.flatten()
predicted = (predicted >= 0.5).astype(int)
predicted[:10]


# In[ ]:


y_test[:10]


# In[ ]:


np.unique(y_test, return_counts=True)


# In[ ]:


np.unique(predicted, return_counts=True)


# In[ ]:


np.unique(predix, return_counts=True)


# In[ ]:


cm=tf.math.confusion_matrix(predicted, y_test)
cm


# In[ ]:


sbn.heatmap(cm, annot=True, fmt='d')
plt.xlabel ('TEST')
plt.ylabel ('PREDICTED')
plt.show()


# In[ ]:


total = []
for i in range (len(cm)):
    Temp_1 = 0
    for j in range (len(cm)):
        Temp_1 = Temp_1 + cm[i, j]
        if (i==j):
            Temp_2 = cm[i,j]
    tot = Temp_2/Temp_1*100
    total.append(tot)
        


# In[ ]:


total=np.array(total)


# In[ ]:


from sklearn.metrics import classification_report

# This prints a table with Precision, Recall, and F1 for BOTH classes (0 and 1)
report = classification_report(y_test, predicted)
print(report)


# In[ ]:


from sklearn.metrics import accuracy_score, recall_score

# Get the raw decimal values
accuracy_score(y_test, predicted)


# In[ ]:





# In[ ]:


test.plot('time', 'ax')
plt.show()


# In[ ]:


test


# In[ ]:


test_e = test[['ax', 'ay', 'az']].values
predix = model.predict(test_e)
test_e.shape


# In[ ]:


predix


# In[ ]:


predix = (predix >= 0.5).astype(int)


# In[ ]:


predix=predix.flatten()


# In[ ]:


np.unique(predix, return_counts=True)


# In[ ]:


np.unique(predix, return_counts=True)


# In[ ]:


fig, ay1 = plt.subplots()

# First X-axis (Bottom)
ay1.plot(test.index, test['ax'], color='blue')
ay1.set_xlabel('X_accelo', color='blue')
ay1.set_ylabel('Time interval')

ay1.plot(test.index, test['ay'], color='green')
ay1.set_xlabel('X_accelo', color='blue')
ay1.set_ylabel('Time interval')

ay1.plot(test.index, test['az'], color='purple')
ay1.set_xlabel('X_accelo', color='blue')
ay1.set_ylabel('Time interval')

# Second X-axis (Top)
ay2 = ay1.twiny() 
ay2.plot(test.index, predix, color='red')
ay2.set_xlabel('Machine status', color='red')
plt.show()


# In[ ]:


predix=pd.DataFrame(predix)


# In[ ]:


predix['time']=test['time'].values
predix.value_counts()


# In[ ]:


predix


# In[ ]:


predix = predix[['time', 0]]
predix = predix.rename(columns={0: 'output'})


# In[ ]:


alpha = np.linspace(0, 42, len(predix))
predix['time'] = alpha


# In[ ]:


predix


# In[ ]:


predix


# In[ ]:


model.summary()


# In[ ]:


model.save("model.keras")


# In[ ]:


converter = tf.lite.TFLiteConverter.from_keras_model(model) #original model with no quantization


# In[ ]:


Lite_model = converter.convert()


# In[ ]:


len(Lite_model)


# In[ ]:


model.summary()


# In[ ]:


with open('Lite_model.tflite', 'wb') as f:
    f.write(Lite_model)


# In[ ]:


converter.optimizations = [tf.lite.Optimize.DEFAULT]
Lite_quant_model = converter.convert()

with open ('Lite_quant_model.tflite', 'wb') as f:
    f.write(Lite_quant_model)
#post quantized model 


# In[ ]:





# In[ ]:


def generate_c_header(bytes_data, var_name):
    hex_array = [f'0x{b:02x}' for b in bytes_data]
    c_str = f'const unsigned char {var_name}[] __attribute__((aligned(4))) = {{\n  '
    c_str += ',\n  '.join([', '.join(hex_array[i:i+12]) for i in range(0, len(hex_array), 12)])
    c_str += f'\n}};\n\nconst unsigned int {var_name}_len = {len(bytes_data)};'
    return c_str

with open ('Converted_model', 'w') as f:
    f.write(generate_c_header(Lite_quant_model, "Converted_model"))


# In[ ]:





# In[ ]:




