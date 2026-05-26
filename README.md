Инструкция по разработке игры «Симулятор парковки» на C++ и SDL2
Эта инструкция предназначена для студентов, приступающих к реализации курсового проекта по созданию двухмерного симулятора парковки автомобиля. В ней подробно описаны архитектурные решения, алгоритмы и практические шаги по написанию кода на языке C++ с использованием графической кроссплатформенной библиотеки SDL2 и её расширений (SDL_image, SDL_ttf).
________________________________________
Введение
Цель работы — разработать игровое приложение «Симулятор парковки», в котором игрок должен аккуратно провести автомобиль через игровое поле, заполненное препятствиями, и припарковать его в заданной зоне за ограниченное время (на сложном уровне).
Инструменты для реализации:
	Среда разработки: Microsoft Visual Studio (рекомендуется версия 2019 или новее).
	Язык программирования: C++ (стандарт C++11 или выше).
	Графическая библиотека: SDL2 (Simple DirectMedia Layer).
	Дополнительные библиотеки:
	SDL_image — для загрузки изображений в форматах PNG/JPG.
	SDL_ttf — для рендеринга шрифтов и вывода текста (таймер, меню, подсказки).
________________________________________
1. Подготовка среды разработки
Перед написанием кода необходимо настроить проект в Visual Studio и подключить библиотеки SDL2.
Пошаговый алгоритм настройки проекта:
	Скачивание библиотек: Перейдите на официальные сайты библиотек и скачайте VC-версии (Development Libraries) для:
	SDL2
	SDL_image
	SDL_ttf
	Структура проекта: Создайте в папке решения директорию dependencies с подпапками include и lib (для x64 или x86 платформ). Распакуйте туда соответствующие файлы заголовков (.h) и библиотек (.lib).
	Настройка свойств проекта в Visual Studio:
	Откройте Свойства проекта -> C/C++ -> Общие -> Дополнительные папки включаемых файлов и укажите путь к dependencies/include.
	Перейдите в Компоновщик -> Общие -> Дополнительные библиотеки и укажите путь к dependencies/lib.
	В разделе Компоновщик -> Ввод -> Дополнительные зависимости добавьте:
text
SDL2.lib; SDL2main.lib; SDL2_image.lib; SDL2_ttf.lib;
	В свойствах проекта в разделе Система установите Подсистема -> Консоль (для удобства отладки через std::cout) или Windows.
	Копирование DLL: Скопируйте все динамические библиотеки (.dll) из скачанных архивов в папку, где будет собираться исполняемый файл .exe (обычно x64/Debug или x64/Release).

2. Разработка архитектуры и класса Car
Автомобиль — ключевой физический объект игры. Класс Car инкапсулирует в себе данные о координатах, угле поворота, текстуре и логике перемещения.
Поля класса Car (из файла Car.h):
	float x, y — точные координаты положения машины на экране (использование типа float критически важно для плавности движения).
	double angle — угол поворота машины в градусах (от 0 до 360).
	float speed — текущая скорость движения.
	SDL_Rect rect — целочисленный прямоугольник для позиционирования и коллизий (координаты округляются при обновлении кадра).
	SDL_Texture* texture — указатель на загруженное изображение автомобиля.
Логика перемещения автомобиля (Car.cpp):
Поскольку исходный спрайт машины по умолчанию направлен вертикально вверх, тригонометрическая система координат сдвигается на 90 градусов. Формула перевода угла из градусов в радианы выглядит так:
"rad"=(("angle" +90.0)×π)/180.0
Реализация методов движения в Car.cpp:
	#define PI 3.14159265358979323846

void Car::moveForward() {
    // Поправка на 90 градусов, так как картинка смотрит вверх
    double rad = (angle + 90.0) * PI / 180.0;
    x = x + speed * (float)cos(rad);
    y = y + speed * (float)sin(rad);
    update();
}

void Car::moveBackward() {
    double rad = (angle + 90.0) * PI / 180.0;
    x = x - speed * (float)cos(rad);
    y = y - speed * (float)sin(rad);
    update();
}

void Car::rotateLeft(float degrees) {
    angle -= degrees;
    if (angle < 0) angle += 360.0;
}

void Car::rotateRight(float degrees) {
    angle += degrees;
    if (angle >= 360.0) angle -= 360.0;
}
Метод Car::update() выполняет синхронизацию точных координат float с экранными пикселями rect.x и rect.y, а также предотвращает вылет автомобиля за пределы экрана.
________________________________________
3. Реализация игрового поля и парковки
Размеры игрового окна жестко заданы в Main.cpp: 1280x731 пикселей.
Ограничение игрового поля:
По периметру поля располагается визуальный бордюр. Для создания физической границы в коде определяется константа ширины рамки:
const int BORDER_WIDTH = 80;

Любое пересечение автомобилем этой зоны интерпретируется как авария (поражение).
Определение зоны парковки:
Парковка представляет собой целевой прямоугольник SDL_Rect parkingZone. Её координаты центра и размеры задаются в функции main:
int parkingCenterX = 1080;
int parkingCenterY = 530;
int parkingW = 150;
int parkingH = 150;

SDL_Rect parkingZone;
parkingZone.x = parkingCenterX - parkingW / 2;
parkingZone.y = parkingCenterY - parkingH / 2;
parkingZone.w = parkingW;
parkingZone.h = parkingH;

4. Создание системы препятствий (Collision Detection)
Препятствия на карте создаются динамически в зависимости от выбранного уровня сложности.
Описание структур данных (Main.cpp):
Для конфигурации препятствий используется облегченная структура данных ObstacleData, хранящая путь к файлу текстуры, координаты центра и размеры хитбокса (зоны коллизии):
struct ObstacleData {
    string file;
    int centerX, centerY;
    int hitW, hitH;
};

Векторы препятствий (obstaclesDataEasy, obstaclesDataMedium, obstaclesDataHard) инициализируются в начале программы. Пример для легкого уровня:
vector<ObstacleData> obstaclesDataEasy = {
    {"res/stena_ob.png", 900, 100, 103, 119},
    {"res/stena_ob.png", 200, 520, 103, 119},
    {"res/tree_ob.png", 600, 300, 100, 100}
};

Алгоритм проверки коллизий:
Для обнаружения столкновения автомобиля с препятствиями применяется алгоритм AABB (Axis-Aligned Bounding Box) — проверка пересечения непересекающихся прямоугольников:
bool checkCollision(const SDL_Rect& carRect, const SDL_Rect& obsRect) {
    return (carRect.x < obsRect.x + obsRect.w &&
            carRect.x + carRect.w > obsRect.x &&
            carRect.y < obsRect.y + obsRect.h &&
            carRect.y + carRect.h > obsRect.y);
}

5. Разработка меню и выбор уровня/машины
Класс Menu состоит из статических методов, что позволяет вызывать интерфейсные окна без создания долгоживущих объектов меню.
Отображение главного меню (Menu.cpp):
Метод Menu::showMainMenu(SDL_Renderer* renderer) отрисовывает кнопки уровней («Easy», «Medium», «Hard») и запускает внутренний цикл ожидания клика мыши:
int Menu::showMainMenu(SDL_Renderer* renderer) {
    SDL_Rect easyRect = { 270, 530, 200, 50 };
    SDL_Rect mediumRect = { 555, 530, 200, 50 };
    SDL_Rect hardRect = { 895, 530, 200, 50 };

    bool selecting = true;
    int selected = 0;
    SDL_Event e;

    while (selecting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return 0;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                if (mouseX >= easyRect.x && mouseX <= easyRect.x + easyRect.w &&
                    mouseY >= easyRect.y && mouseY <= easyRect.y + easyRect.h) {
                    selected = 1; selecting = false;
                }
                // Аналогичные проверки для mediumRect (selected = 2) и hardRect (selected = 3)
            }
        }
        // Отрисовка фона меню и текста поверх кнопок...
        SDL_RenderPresent(renderer);
    }
    return selected;
}

Аналогичным образом устроен метод Menu::showCarSelection(), который считывает выбор цвета машины из четырех доступных вариантов и возвращает индекс выбранного спрайта.
________________________________________
6. Реализация игрового цикла и обработки ввода
Функция runLevel содержит главный игровой цикл уровня. Чтобы управление автомобилем было отзывчивым и плавным, события клавиатуры изменяют флаги состояния кнопок (keyUp, keyDown, keyLeft, keyRight).
bool keyUp = false;
bool keyDown = false;
bool keyLeft = false;
bool keyRight = false;

Структура игрового цикла:
while (levelActive) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 2; // Выход из игры
        }
        
        if (!win && !lose) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP) keyUp = true;
                else if (event.key.keysym.sym == SDLK_DOWN) keyDown = true;
                else if (event.key.keysym.sym == SDLK_LEFT) keyLeft = true;
                else if (event.key.keysym.sym == SDLK_RIGHT) keyRight = true;
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_UP) keyUp = false;
                else if (event.key.keysym.sym == SDLK_DOWN) keyDown = false;
                else if (event.key.keysym.sym == SDLK_LEFT) keyLeft = false;
                else if (event.key.keysym.sym == SDLK_RIGHT) keyRight = false;
            }
        }
    }

    // Шаг физики и перемещения (если игра активна)
    if (!win && !lose) {
        float oldX = car.getX();
        float oldY = car.getY();

        if (keyUp) car.moveForward();
        if (keyDown) car.moveBackward();
        if (keyLeft) car.rotateLeft(rotationSpeed);
        if (keyRight) car.rotateRight(rotationSpeed);
        
        // Далее следуют проверки на коллизии и победу
    }
    
    // Очистка экрана, рендеринг объектов, вывод текста и вызов SDL_RenderPresent...
    SDL_Delay(16); // Ограничение ~60 FPS
}

7. Реализация логики победы/поражения и таймера
Проверка условий проигрыша:
Столкновение фиксируется, если хитбокс автомобиля перекрывает любое препятствие или выходит за пределы рамки игрового поля:
// Выход за границы рамки BORDER_WIDTH
SDL_Rect carRect = car.getRect();
if (carRect.x < BORDER_WIDTH || 
    carRect.x + carRect.w > 1280 - BORDER_WIDTH ||
    carRect.y < BORDER_WIDTH || 
    carRect.y + carRect.h > 731 - BORDER_WIDTH) {
    collision = true;
}

if (collision) {
    lose = true;
}

Проверка условий победы:
Для успешного прохождения уровня игрок должен полностью заехать в зону парковки. Все четыре вершины прямоугольника автомобиля должны находиться внутри parkingZone:
if (!lose &&
    carRect.x >= parkingZone.x && 
    carRect.x + carRect.w <= parkingZone.x + parkingZone.w &&
    carRect.y >= parkingZone.y && 
    carRect.y + carRect.h <= parkingZone.y + parkingZone.h) {
    win = true;
}

Логика таймера для уровня Hard (Сложный):
Ограничение времени составляет 45 секунд и действует только на третьем уровне. Отсчет времени реализуется с использованием встроенной функции SDL_GetTicks(), возвращающей время в миллисекундах с момента запуска приложения.
if (!win && !lose && level == 3 && timeLimit > 0.0f) {
    float elapsed = (SDL_GetTicks() - startTime) / 1000.0f;
    if (elapsed >= timeLimit) {
        lose = true; // Время вышло
    }
}

Отображение таймера:
Оставшееся время рассчитывается каждую итерацию цикла. Если на таймере осталось меньше 10 секунд, цвет шрифта меняется на красный для привлечения внимания:
float remaining = timeLimit - elapsed;
SDL_Color textColor = (remaining < 10) ? SDL_Color{255, 50, 50, 255} : SDL_Color{255, 255, 255, 255};
string timeText = "Time: " + to_string((int)remaining / 60) + ":" + to_string((int)remaining % 60);
Menu::drawText(renderer, timeText, 1000, 20, textColor);

8. Сборка и тестирование
После реализации кода необходимо проверить соответствие параметров физики игры техническому заданию:
Уровень сложности	Скорость движения (speed)	Скорость поворота (rotationSpeed)	Кол-во препятствий	Наличие таймера
Easy (Легкий)	2.5 у.е./кадр	4.0 у.е./кадр	3 шт.	Нет
Medium (Средний)	4.0 у.е./кадр	5.0 у.е./кадр	6 шт.	Нет
Hard (Сложный)	6.0 у.е./кадр	6.0 у.е./кадр	11 шт.	Есть (45 сек)

Для отладки используйте вывод в консоль координат автомобиля при столкновениях. Убедитесь, что при нажатии клавиши R уровень корректно перезапускается без утечек памяти (все ранее созданные текстуры препятствий должны удаляться вызовом SDL_DestroyTexture).
________________________________________
Заключение (Критические моменты)
При самостоятельной реализации проекта обратите особое внимание на следующие нюансы:
	Освобождение ресурсов (Memory Cleanup): Напишите деструкторы для классов Car и Obstacle. В конце работы программы или при перезапуске уровня обязательно вызывайте SDL_DestroyTexture для всех загруженных текстур и SDL_FreeSurface для временных поверхностей. Невыполнение этого требования ведет к утечке видеопамяти.
	Центр поворота текстуры: При вызове функции рендеринга SDL_RenderCopyEx для автомобиля, передавайте относительные координаты центра center.x = rect.w / 2 и center.y = rect.h / 2. Если этого не сделать, машина будет неестественно вращаться вокруг своего левого верхнего угла.
	Использование типов с плавающей точкой: Все расчеты перемещения (x += speed * cos(rad)) должны проводиться строго в переменных типа float или double. Перевод в целочисленный пиксельный формат (SDL_Rect) должен происходить только на этапе обновления экранных координат в методе update(). Это гарантирует плавность хода машины при низких скоростях.

