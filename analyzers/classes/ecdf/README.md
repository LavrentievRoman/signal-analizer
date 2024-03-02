# Анализатор ECDF

ECDF stands for Empirical distribution function. По русски -- выборчная (эмпирическая) функция распределения.

На основе данных обучающей выборки вычисляет параметры функции распределения. Для анализируемого фрагмента вычисляет вероятность принадлежности данного фрагмента данному распределению. Если вероятность принадлежности ниже пороговой, то фрагмент считается аномальным.

Настраиваемые гиперпараметры:
* размер фрагмента (целое).
* пороговая вероятность (с плавающей точкой).

Особенности использования:
* если обучающая выборка недостаточного велика для решения системы уравнений, то обучение завершается с ошибкой.