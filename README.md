## Lavash

Вам предстоить написать свой аналог shell под названием lavash на языке C++.

Решение задания необходимо сдать в свои репозитории в ветку master. За задания полагаются частичные баллы. 
Все тесты открытые. Критерием баллов являются сами тесты.

Почти все что вы сделаете будет совместимо с обычным bash, однако на сервере вы не сможете использовать этот и другие встроенные интерпретаторы.


### 1. База

Напишите свой простой парсер аргументов командной строки.
Идеальное решение должно поддерживать в том числе аргументы в кавычках и экранирование (см тесты).

Реализуйте поддержку опции "-c" вашей программы, принимающую на вход аргументом строку (в том числе содержащую пробелы) 
являющуюся командой для исполнения.

Используя системные вызовы fork, exec, wait* и другие реализуйте простой запуск подпрограммы и дождитесь её завершения. 
Код возврата вашей программы должен соответствовать коду возврата запущенного приложения.


### 2. Pipe

Реализуйте оператор `|` соединяющий вывод предыдущей команды со входом следующей через pipe

### 3. Files

Реализуйте поддержку ввода/вывода из файлов с помощью операторов `<` и `>`.

### 4. TODO операторы `&&` `||` итд