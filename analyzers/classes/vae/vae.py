from tensorflow import keras
from tensorflow.keras import layers
import numpy as np

class FitProgessCallbacks(keras.callbacks.Callback):
    
    def __init__(self, epoch_count, batch_count, fit_begin_progress_value, fit_end_progress_value):
        self.epoch_count = epoch_count
        self.batch_count = batch_count
        self.begin_progress_value = fit_begin_progress_value
        self.progress_per_epoch = (fit_end_progress_value - fit_begin_progress_value) / epoch_count
        self.current_epoch = 0
        self.current_batch = 0
    
    def on_epoch_begin(self, epoch, logs=None):
        self.current_epoch = epoch
        self.current_batch = 0
        
    def on_train_batch_begin(self, batch, logs=None):
        self.current_batch = batch
        on_fit_progress(self.begin_progress_value + self.progress_per_epoch * (self.current_epoch + self.current_batch / self.batch_count))
        
class EstimateProgessCallbacks(keras.callbacks.Callback):
    
    def __init__(self, batch_count, estimate_begin_progress_value, estimate_end_progress_value):
        self.batch_count = batch_count
        self.begin_progress_value = estimate_begin_progress_value
        self.progress_per_batch = (estimate_end_progress_value - estimate_begin_progress_value) / batch_count
        
    def on_predict_batch_begin(self, batch, logs=None):
        self.current_batch = batch
        on_fit_progress(self.begin_progress_value + self.progress_per_batch * self.current_batch)

class VaeModel:

    #def __init__(self, count_interval_size = 256, count_step_size = 1):
    #    self.__count_interval_size = count_interval_size
    #    self.__count_step_size = count_step_size
    def __init__(self, inputSize = 256):
        self.__count_interval_size = inputSize
        self.__count_step_size = 1

        self.__model = keras.Sequential(
            [
                #layers.Input(shape=(count_interval_size, count_step_size)), # (10, 1)
                layers.Input(shape=(self.__count_interval_size, self.__count_step_size)), # (10, 1)

                layers.Conv1D(filters=2, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),
                layers.Conv1D(filters=3, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),
                layers.Conv1D(filters=5, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),
                layers.Conv1D(filters=8, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),


                layers.Conv1DTranspose(filters=5, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),
                layers.Conv1DTranspose(filters=3, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),
                layers.Conv1DTranspose(filters=2, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),
                layers.Conv1DTranspose(filters=1, kernel_size=7, padding="same", strides=2, activation="elu", kernel_initializer = 'he_uniform'),

                layers.Conv1DTranspose(filters=1, kernel_size=7, padding="same"),
            ]
        )
        self.__model.compile(optimizer=keras.optimizers.Adam(learning_rate=0.001), loss="mse") # Adam, Adamax, Nadam
        print(self.__model.summary())

    #def fit(self, series, count_interval_size, count_step_size):
    def fit(self, series):
        on_fit_progress(0.0)
        
        self.__mean = np.mean(series)
        self.__std = np.std(series)
        series_norm = (series - self.__mean) / self.__std
        #series_norm = self.__create_sequences(series_norm, count_interval_size, count_step_size)
        series_norm = self.__create_sequences(series_norm, self.__count_interval_size, self.__count_step_size)

        df_training_value = series_norm.reshape((
            series_norm.shape[0],
            series_norm.shape[1],
            1))

        cp_callback = keras.callbacks.ModelCheckpoint(
            filepath = 'temp/rnn_rnn_vae_model.h5', save_weights_only = False,
            verbose = 1, save_best_only = True
        )
        cp_callback_1 = keras.callbacks.EarlyStopping(monitor="val_loss", patience=5, mode="min")

        on_fit_progress(0.2)

        print("len(series) = " + str(len(series)))
        batch_size = 64
        validation_split = 0.1
        batch_count = len(series) * (1.0 - validation_split) / batch_size
        epochs = 3

        history = self.__model.fit(
            df_training_value,
            df_training_value,
            epochs=3,
            batch_size=64,
            validation_split=0.1,
            callbacks=[
                cp_callback, cp_callback_1, FitProgessCallbacks(epochs, batch_count, 0.2, 0.8)
            ]
        )
        
        batch_size = 16
        batch_count = len(series) / (2 * batch_size)
        
        on_fit_progress(0.8)

        predict = self.__model.predict(series_norm, callbacks = [ EstimateProgessCallbacks(batch_count, 0.8, 1.0) ])
        diff = np.abs(df_training_value - predict)
        self.__threshold = np.quantile(diff, 0.999)
        
        on_fit_progress(1.0)

    def estimate(self, series):
        series_norm = (series - self.__mean) / self.__std
        predict = self.__model.predict(series_norm.reshape((1, self.__count_interval_size, 1)))
        diff = np.abs(series_norm - predict)
        anomalies = diff > self.__threshold
        res = np.mean(anomalies)
        return res

    def __create_sequences(self, values, count_interval_size, count_step_size):
        assert isinstance(values, np.ndarray)
        values_shape = values.shape
        assert len(values_shape) == 1
        values_len = values_shape[0]
        output = []
        for i in range(0, values_len - count_interval_size + 1, count_step_size):
            output.append(values[i : (i + count_interval_size)])
        return np.stack(output)

def vae_loss2(input_x, decoder1, z_log_sigma, z_mean):
    """ Calculate loss = reconstruction loss + KL loss for each data in minibatch """
    # E[log P(X|z)]
    recon = K.sum(K.binary_crossentropy(input_x, decoder1))
    # D_KL(Q(z|X) || P(z|X)); calculate in closed form as both dist. are Gaussian
    kl = 0.5 * K.sum(K.exp(z_log_sigma) + K.square(z_mean) - 1. - z_log_sigma)
    return recon + kl

def sampling(args):
    z_mean, z_log_sigma = args
    batch_size = tf.shape(z_mean)[0] # <================
    epsilon = K.random_normal(shape=(batch_size, 10), mean=0., stddev=1.)
    return z_mean + z_log_sigma * epsilon
