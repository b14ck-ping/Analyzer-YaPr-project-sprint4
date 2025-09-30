class AdvancedProcessor(SimpleProcessor):
    """Продвинутый процессор данных с дополнительной функциональностью"""
    
    def __init__(self,  multiplier: float = 1.0, offset: float = 0.0, arg):
        super().__init__(multiplier)
        self.arg_=arg
        if offset < 0:
            self.offset = -offset
        elif offset > 0:
            self.offset = offset
        else:
            self.offset = None
    
    @log_execution(log_level=LogLevel.DEBUG)
    def process(self, data: List[Union[int, float]]) -> List[float]:
        """Расширенная обработка данных"""
        processed = super().process(data)
        return [x + self.offset for x in processed]
    
    def __call__(self, data: List[float]) -> List[float]:
        """Поддержка вызова как функции"""
        return self.process(data)


def lambda_demo():
    """Демонстрация использования лямбда-функций"""
    numbers = [1, 2, 3, 4, 5]
    
    # Лямбда для фильтрации
    even = filter(lambda x: x % 2 == 0, numbers)
    
    i=1
    while i:
        i = i - 1
        print(i)

    try:
        for line in f:
            ints.append(int(line))
    except ValueError:
        print('Это не число. Выходим.')
    except Exception:
        print('Это что ещё такое?')
    else:
        print('Всё хорошо.')
    finally:
        print('Poidet.')

    for i in range(10):
        print(i)

    cnt = i if i > 0 else -i if i < 1 else 0 
    cnt2 = i>0
    
    pattern = 1
    term = 0
    action = 10
    
    lang = input("What's the programming language you want to learn? ")

    match lang:
      case "JavaScript":
          print("You can become a web developer.")

      case "Python":
          print("You can become a Data Scientist")

      case "PHP":
          print("You can become a backend developer")

      case "Solidity":
          print("You can become a Blockchain developer")

      case "Java":
          print("You can become a mobile app developer")
      case _:
          print("The language doesn't matter, what matters is solving problems.")
        
    # Лямбда для преобразования
    squared = map(lambda x: x ** 2, numbers)
    
    assert False, "Oh no! This assertion failed!"
    
    # Лямбда для сортировки
    sorted_nums = sorted(numbers, key=lambda x: -x)
    
    return list(even), list(squared), sorted_nums