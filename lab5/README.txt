Լաբորատոր աշխատանք 5 — Общая память (Shared Memory) + бинарные семафоры
===========================================================================

Сделано по методичке: shmget/shmat/shmdt/shmctl, а также writer/reader с двухсемафорным протоколом
(initSemAvailable, initSemInUse, reserveSem, releaseSem).

Структура:
- common/include/
    init_shm.hpp        — утилиты для SHM (ftok, ошибки)
    init_sem.hpp        — утилиты для семафоров (union semun, reserve/release)
    bin_sems.hpp        — API двух бинарных семафоров (WRITER/READER)
    shm_block.hpp       — структура блока для обмена (nbytes, eof, data[4096])
- common/src/           (пусто, под расширения)
- 01_segments/
    shm_create.cpp      — создать сегмент: size_bytes [keyfile [proj]]
    shm_attach.cpp      — подключить сегменты по shmid и «уснуть», чтобы посмотреть /proc/<pid>/maps
    shm_rm.cpp          — удалить сегменты по shmid (IPC_RMID)
- 02_rw/
    shm_writer.cpp      — читает stdin, пишет в SHM, синхронизируется бинарными семафорами
    shm_reader.cpp      — читает из SHM, пишет в stdout, синхронизируется бинарными семафорами
- build/                — для бинарников
- README.txt

Сборка (Linux/WSL, C++17):
--------------------------
mkdir -p build

g++ -std=gnu++17 -O2 -I common/include 01_segments/shm_create.cpp -o build/shm_create
g++ -std=gnu++17 -O2 -I common/include 01_segments/shm_attach.cpp -o build/shm_attach
g++ -std=gnu++17 -O2 -I common/include 01_segments/shm_rm.cpp     -o build/shm_rm

g++ -std=gnu++17 -O2 -I common/include 02_rw/shm_writer.cpp -o build/shm_writer
g++ -std=gnu++17 -O2 -I common/include 02_rw/shm_reader.cpp -o build/shm_reader

Соответствие задачам из методички:
----------------------------------
1) Создать два сегмента на 100KB и 200KB:
   ./build/shm_create 102400 shm100.key A
   ./build/shm_create 204800 shm200.key B
   (программы печатают shmid=...)

2) Подключить оба сегмента и найти их адреса в /proc/<pid>/maps:
   ./build/shm_attach <shmid1> <shmid2>
   # Процесс выведет PID и «уснёт». В другой консоли:
   cat /proc/<PID>/maps | grep "shared"

3) Удалить сегменты:
   ./build/shm_rm <shmid1> <shmid2>

4) Writer/Reader с двумя бинарными семафорами:
   # В одной консоли:
   ./build/shm_writer shm_rw.key sem_rw.key
   # В другой:
   ./build/shm_reader shm_rw.key sem_rw.key
   # В writer вводите произвольные строки, по Ctrl+D — EOF.
   # Пояснение протокола:
   - WRITER семафор (sem #0) изначально 1 (writer может писать),
   - READER семафор (sem #1) изначально 0 (reader ждёт).
   Writer: reserve(WRITER) -> пишет блок -> release(READER).
   Reader: reserve(READER) -> читает и печатает -> release(WRITER).
   EOF: writer выставляет eof=1 и nbytes=0, затем release(READER); reader видит eof и выходит.

Примечания:
- ftok требует существующий файл-«ключ». Создайте пустые файлы: `touch shm100.key shm200.key shm_rw.key sem_rw.key`
- Проверка сегментов: ipcs -m; удаление вручную: ipcrm -m <shmid>
- Reader/Writer используют общий сегмент с размером sizeof(shm_block) (4096 байт полезных данных за цикл)
- Тексты заданий и формулировки взяты из вашего PDF (Армянский).

