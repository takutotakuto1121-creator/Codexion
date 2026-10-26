*This project has been created as part of the 42 curriculm by tsugimot.*
# Codexion (English ver.)
## Description
### Overview
Codexion is a solo project for Milestone 3 of the 42cursus. It is a variation of the famous "Dining Philosophers" problem. Coders sit in a circle, and between each of them, dongles are also placed in a circle. Each coder must acquire the dongles on both their left and right sides to compile their code. Afterward, they release the dongles, debug, and refactor. Each dongle can only be held by one coder at a time, specifically by the coders sitting directly on either side of it. Additionally, once a dongle is acquired and used for compiling, it goes into a cooldown period during which no coder can acquire it.

### Details
The program takes the following information as arguments:
1. `number_of_coders`: The number of coders and also the number of dongles.
2. `time_to_burnout`: If a coder does not compile within `time_to_burnout` milliseconds since the beginning of their last compilation (or the start of the simulation if they haven't compiled yet), the coder will burn out, and the program will terminate. The unit is in milliseconds.
3. `time_to_compile`: The time it takes for a coder to compile. The unit is in milliseconds.
4. `time_to_debug`: The time it takes for a coder to debug. The unit is in milliseconds.
5. `time_to_refactor`: The time it takes for a coder to refactor. The unit is in milliseconds.
6. `number_of_compiles_required`: If all coders have compiled at least `number_of_compiles_required` times, the program will terminate.
7. `dongle_cooldown`: The time it takes for a dongle to cool down. The unit is in milliseconds.
8. `scheduler`: Either `fifo` or `edf`. Specifies the rule applied when multiple coders want to acquire the same dongle at the same time.
   - `fifo` (First-In, First-Out): Prioritizes the coder who became ready to acquire the dongle first.
   - `edf` (Earliest Deadline First): Prioritizes the coder with the shortest time left until burnout.

Each coder is assigned an ID ranging from 1 to `number_of_coders`. Coder No. 1 sits next to Coder No. `number_of_coders`. Coder No. N sits between Coder No. N-1 and Coder No. N+1.
Logs are output to the standard output in the following format:
```bash
# <timestamp_in_ms> <ID> <action>
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
402 2 has taken a dongle
403 2 has taken a dongle
403 2 is compiling
603 2 is debugging
803 2 is refactoring
1204 3 burned out
```
The goal is to create a program that sequentially outputs the actions of the coders under the constraints described above.

## Instruction (Usage)
Clone the repository from the remote URL:
```bash
git clone [https://github.com/takutotakuto1121-creator/Codexion.git](https://github.com/takutotakuto1121-creator/Codexion.git) codexion
```
Move into the cloned directory:
```bash
cd codexion
```
Compile the code using the following command to create the executable file `codexion`:
```bash
make
```
Execute the program. The arguments from left to right correspond to items 1 through 8 in the Description/Details section. These can be modified freely. The values below are just an example:
```bash
./codexion 4 800 200 200 200 5 10 fifo
```

## Blocking cases handled
A deadlock occurs only when the following four conditions are met simultaneously. These are known as the Coffman conditions:

1. **Mutual Exclusion:** A resource can only be used by one process at a time.
2. **Hold and Wait:** A process holds a resource while waiting for another resource to be released.
3. **No Preemption:** A resource held by another process cannot be forcibly taken away.
4. **Circular Wait:** Multiple processes are waiting for each other's resources in a circular chain.

Conversely, if even one of these conditions is broken, a deadlock will not occur.

For clear examples of specific countermeasures, please refer to [this article](https://qiita.com/gts/items/11a113cbb3eef893d9fe).

In the context of this assignment, there are two places where a deadlock can occur.

First: Suppose a situation where all coders simultaneously acquire their left dongle and wait for their right dongle to become available. A deadlock occurs from this state. We resolved this by breaking the circular wait condition: we implemented it so that only the coder with the ID `number_of_coders` acquires the right dongle first and then the left dongle, while all other coders acquire their left dongle first.

Second: When the number of coders is 1, the left and right values inside `take_dongle_fifo` and `take_dongle_edf` end up being the same. As a result, when attempting to acquire the dongles in `wait_and_lock_dongles`, the program tries to lock the exact same dongle for both right and left, which causes a deadlock. We resolved this by treating the case where `number_of_coders` is 1 as a special exception and handling it separately.

## Thread synchronization mechanisms
In addition to the main thread, the program uses one thread per coder and one additional thread for monitoring.

### Coder Threads
They repeat the following loop:
1. Acquire dongles. If no dongles are available, wait until they become available.
2. Compile, and then release the dongles.
3. Debug, and then refactor.
4. If the termination conditions are met, end the thread. If not, return to step 1.

### Monitor Thread
It repeats the following loop:
1. If any coder meets the burnout criteria, detect it, send a termination signal to the coder threads, and then terminate itself.
2. If all coders have compiled at least `number_of_compiles_required` times, detect it, send a termination signal to the coder threads, and terminate itself. If not, return to step 1.

## Resources
### Technologies Used
The program uses the `pthread` library. I utilized the functions listed in [memo](memo.md). Please excuse the readability, as it is just a personal memo.

### Use of AI
The foundation of the code was entirely written by hand. I did not use AI for the core logic.
However, I used AI to resolve `norminette` (42's coding standard) errors, as this is not the core focus of a Milestone 3 project. Additionally, while I wrote the Japanese version of the README.md by hand, this English version was created by having an AI translate the Japanese text.

### References
[Explanation of multithreading in C](https://daeudaeu.com/multithread/)
[How to access arrays cyclically using the modulo operator](https://www.cc.kyoto-su.ac.jp/~yamada/ap/cyclicIndex.html)
[explanation of deadlock](https://qiita.com/gts/items/11a113cbb3eef893d9fe)
