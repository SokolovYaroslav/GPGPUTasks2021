#include <CL/cl.h>
#include <libclew/ocl_init.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>


template <typename T>
std::string to_string(T value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

void reportError(cl_int err, const std::string &filename, int line)
{
    if (CL_SUCCESS == err)
        return;

    // Таблица с кодами ошибок:
    // libs/clew/CL/cl.h:103
    // P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
    std::string message = "OpenCL error code " + to_string(err) + " encountered at " + filename + ":" + to_string(line);
    throw std::runtime_error(message);
}

#define OCL_SAFE_CALL(expr) reportError(expr, __FILE__, __LINE__)


int main()
{
    // Пытаемся слинковаться с символами OpenCL API в runtime (через библиотеку libs/clew)
    if (!ocl_init())
        throw std::runtime_error("Can't init OpenCL driver!");

    // Откройте 
    // https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/
    // Нажмите слева: "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformIDs"
    // Прочитайте документацию clGetPlatformIDs и убедитесь, что этот способ узнать, сколько есть платформ, соответствует документации:
    cl_uint platformsCount = 0;
    OCL_SAFE_CALL(clGetPlatformIDs(0, nullptr, &platformsCount));
    std::cout << "Number of OpenCL platforms: " << platformsCount << std::endl;

    // Тот же метод используется для того, чтобы получить идентификаторы всех платформ - сверьтесь с документацией, что это сделано верно:
    std::vector<cl_platform_id> platforms(platformsCount);
    OCL_SAFE_CALL(clGetPlatformIDs(platformsCount, platforms.data(), nullptr));

    for (int platformIndex = 0; platformIndex < platformsCount; ++platformIndex) {
        std::cout << "Platform #" << (platformIndex + 1) << "/" << platformsCount << std::endl;
        cl_platform_id platformId = platforms[platformIndex];

        // Откройте документацию по "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformInfo"
        // Не забывайте проверять коды ошибок с помощью макроса OCL_SAFE_CALL
        // TODO 1.1
        // Попробуйте вместо CL_PLATFORM_NAME передать какое-нибудь случайное число - например 239
        // Т.к. это некорректный идентификатор параметра платформы - то метод вернет код ошибки
        // Макрос OCL_SAFE_CALL заметит это, и кинет ошибку с кодом
        // Откройте таблицу с кодами ошибок:
        // libs/clew/CL/cl.h:103
        // P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
        // Найдите там нужный код ошибки и ее название
        // Затем откройте документацию по clGetPlatformInfo и в секции Errors найдите ошибку, с которой столкнулись
        // в документации подробно объясняется, какой ситуации соответствует данная ошибка, и это позволит, проверив код, понять, чем же вызвана данная ошибка (некорректным аргументом param_name)
        // Обратите внимание, что в этом же libs/clew/CL/cl.h файле указаны всевоможные defines, такие как CL_DEVICE_TYPE_GPU и т.п.
        // OCL_SAFE_CALL(clGetPlatformInfo(platform_id, 239, 0, nullptr, nullptr));
        // ANSWER: Кинуло -30: CL_INVALID_VALUE

        // TODO 1.2
        // Аналогично тому, как был запрошен список идентификаторов всех платформ - так и с названием платформы, теперь, когда известна длина названия - его можно запросить:
        size_t platformNameSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, nullptr, &platformNameSize));
        std::vector<unsigned char> platformName(platformNameSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platformId, CL_PLATFORM_NAME, platformNameSize, platformName.data(), nullptr));
        std::cout << "    Platform name: " << platformName.data() << std::endl;

        // TODO 1.3
        // Запросите и напечатайте так же в консоль вендора данной платформы
        size_t vendorNameSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platformId, CL_PLATFORM_VENDOR, 0, nullptr, &vendorNameSize));
        std::vector<unsigned char> vendorName(vendorNameSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platformId, CL_PLATFORM_VENDOR, vendorNameSize, vendorName.data(), nullptr));
        std::cout << "    Vendor name: " << vendorName.data() << std::endl;

        // TODO 2.1
        // Запросите число доступных устройств данной платформы (аналогично тому, как это было сделано для запроса числа доступных платформ - см. секцию "OpenCL Runtime" -> "Query Devices")
        cl_uint devicesCount = 0;
        OCL_SAFE_CALL(clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, nullptr, &devicesCount));
        std::cout << "    Number of devices on " << platformName.data() << " platform: " << devicesCount << std::endl;

        std::vector<cl_device_id> deviceIds(devicesCount);
        OCL_SAFE_CALL(clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, devicesCount, deviceIds.data(), nullptr));

        for (int deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
            // TODO 2.2
            // Запросите и напечатайте в консоль:
            // - Название устройства
            // - Тип устройства (видеокарта/процессор/что-то странное)
            // - Размер памяти устройства в мегабайтах
            // - Еще пару или более свойств устройства, которые вам покажутся наиболее интересными
            std::cout << "    Device #: " << deviceIndex + 1 << "/" << devicesCount << std::endl;
            cl_device_id deviceId = deviceIds[deviceIndex];

            // Название
            size_t nameSize = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 0, nullptr, &nameSize));
            std::vector<unsigned char> deviceName(nameSize, 0);
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_NAME, nameSize, deviceName.data(), nullptr));
            std::cout << "        Device name: " << deviceName.data() << std::endl;

            // Тип устройства
            cl_device_type deviceType;
            std::string deviceTypeName;
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, nullptr));
            switch (deviceType) {
                case CL_DEVICE_TYPE_CPU:
                    deviceTypeName = "CPU";
                    break;
                case CL_DEVICE_TYPE_GPU:
                    deviceTypeName = "GPU";
                    break;
                case CL_DEVICE_TYPE_DEFAULT:
                    deviceTypeName = "DEFAULT???";
                    break;
                case CL_DEVICE_TYPE_ACCELERATOR:
                    deviceTypeName = "ACCELERATOR";
                    break;
            }
            std::cout << "        Device type: " << deviceTypeName << std::endl;

            // Размер памяти
            cl_ulong memorySize;
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memorySize), &memorySize, nullptr));
            double memorySizeGB = (double) memorySize / (1 << 30);
            std::cout << "        Device memory size in GB: " << memorySizeGB << std::endl;

            // Кажется у меня когда-то были проблемы с тем, что у NVidia big endian... (не уверен)
            cl_bool isLittleEnd;
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_ENDIAN_LITTLE, sizeof(isLittleEnd), &isLittleEnd, nullptr));
            std::string littleEnd = isLittleEnd ? "Little endian device" : "Big endian device";
            std::cout << "        " << littleEnd << std::endl;
            // В итоге у меня всё Little endian, так что мб я напутал

            // Частота
            cl_uint freq;
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(freq), &freq, nullptr));
            std::cout << "        Device frequency: " << freq << " MHz" << std::endl;

            // number of parallel compute units. Как я понимаю это аналог NVidia'вских SM'ов
            cl_uint compUnits;
            OCL_SAFE_CALL(clGetDeviceInfo(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compUnits), &compUnits, nullptr));
            std::cout << "        Device's compute units: " << compUnits << std::endl;
        }
    }

    return 0;
}
