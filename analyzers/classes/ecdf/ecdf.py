from statsmodels.distributions.empirical_distribution import ECDF
import numpy as np


class EcdfModel:
    """
    Searching for anomalies in one-dimensional time series

    Initialization parameters:
        inputSize (int): width of the window to analyze
        alpha (double, from 0 to 1): critical p-value for descriptive statistics
        threshold (double, from 0 to 1): critical value for signaling an anomaly on smoothed series

    Returns:
        numpy array of the same length that input series containing anomaly labels - 0 (OK) or 1(anomaly)
    """

    def __init__(self, inputSize = 480, alpha = 0.01, threshold = 0.9):
        self.__window = inputSize
        self.__alpha = alpha
        self.__threshold = threshold

    def __invchol(self, C):
        Q = np.linalg.inv(np.linalg.cholesky(C))
        return Q

    def __cov(self, y):
        n_points = y.shape[0]
        n_effective_residuals = n_points - self.__window + 1
        indices = (np.tile(np.arange(n_effective_residuals), self.__window).reshape(self.__window,
                                                                                    n_effective_residuals) +
                   np.arange(self.__window)[:, np.newaxis])
        unconditional_cov = np.cov(y[indices])
        return unconditional_cov

    def __residuals(self, y):
        n_points = y.shape[0]
        indices = (np.tile(np.arange(self.__window), n_points - self.__window + 1).reshape(n_points - self.__window + 1,
                                                                                           self.__window) +
                   np.arange(n_points - self.__window + 1)[:, np.newaxis])
        expanded_y = y[indices]
        e = np.dot(expanded_y, self.__Q.transpose())
        return e

    def __testStatistics(self, e):
        t = np.mean(e, axis=1) / (np.std(e, axis=1, ddof=1) / np.sqrt(self.__window))
        chi2 = np.sum(np.power(e, 2), axis=1)
        return np.hstack((t[:, np.newaxis], chi2[:, np.newaxis]))

    def __pvalues(self, statistics):
        t_ecdf = ECDF(self.__stats[:, 0])
        chi2_ecdf = ECDF(self.__stats[:, 1])
        t_values = t_ecdf(statistics[:, 0])
        chi2_values = chi2_ecdf(statistics[:, 1])
        return np.hstack((t_values[:, np.newaxis], chi2_values[:, np.newaxis]))

    def __mark(self, pvalues):
        n_statistics = pvalues.shape[1]
        effective_alpha = self.__alpha / n_statistics
        p_low = effective_alpha / 2
        p_high = 1 - effective_alpha / 2
        result = np.logical_or(np.logical_or(pvalues[:, 0] > p_high, pvalues[:, 0] < p_low),
                               np.logical_or(pvalues[:, 1] > p_high, pvalues[:, 1] < p_low))
        return result

    def __smooth(self, marks):
        n_additional_values = self.__window - 1
        n_points = marks.shape[0] + self.__window - 1
        full_marks = np.concatenate((np.tile(np.nan, n_additional_values),
                                     marks,
                                     np.tile(np.nan, n_additional_values)))
        indices = (np.tile(np.arange(self.__window), n_points).reshape(n_points, self.__window) +
                   np.arange(n_points)[:, np.newaxis])
        expanded_marks = full_marks[indices]
        smoothed_marks = np.nanmean(a=expanded_marks, axis=1)
        return smoothed_marks > self.__threshold

    def fit(self, series):
        length = len(series)

        if length < self.__window + 1:
            raise ValueError("Input series of length {} is too short to calculate " +
                             "autocovariance up to lag {}"
                             .format(length, self.__window))

        on_fit_progress(0.00)
        self.__mean = np.mean(series)
        on_fit_progress(0.17)
        __series = series - self.__mean
        on_fit_progress(0.34)
        cov = self.__cov(__series)
        on_fit_progress(0.51)
        self.__Q = self.__invchol(cov)
        on_fit_progress(0.68)
        e = self.__residuals(__series)
        on_fit_progress(0.85)
        self.__stats = self.__testStatistics(e)
        on_fit_progress(1.00)

    def estimate(self, series):
        length = len(series)

        if length < self.__window:
            raise ValueError("Input series of length {} is too short for window {}"
                             .format(length, self.__window))

        __series = series - self.__mean
        e = self.__residuals(__series)
        t = self.__testStatistics(e)
        p = self.__pvalues(t)
        m = self.__mark(p)
        return m
        # TODO: [Evgeny] it looks like __smooth is useless in our case
        # __smooth([0]) returns [0, 0, ... , 0]
        # __smooth([1]) returns [1, 1, ... , 1]
        # s = self.__smooth(m)
        # return s
