*This project has been created as part of the 42 curriculm by tsugimot.*
# Codexion(日本語ver.)
## Description
### 概要
Codexionは42cursusのMilestone3のソロ課題です。有名問題である、食事する哲学者問題(Dining Philosophers)の類題です。コーダーが円状に並び、それぞれの間にはドングルが同じく円状に配置されています。各コーダーは自分の両隣のドングルを確保し、コンパイルをします。その後、ドングルを離し、デバッグ、リファクタリングを行います。各ドングルは同時に１名までのコーダー、自分の両隣のコーダーにしか確保されません。また、確保され、コンパイルを行ったのち、所定のクールダウンの時間中はいかなるコーダーにも確保されません。
### 詳細
プログラムは以下の情報を受け取ります。
1. `number_of_coders`: コーダー及びドングルの数。
2. `time_to_burnout`: コーダーの最後のコンパイル時刻(まだ一度もコンパイルしていない場合はスタート時刻)から`time_to_burnout`だけ時間が経つと、コーダーはburnoutし、プログラムは終了する。単位はmilliseconds。
3. `time_to_compile`: コーダーがコンパイルするのにかかる時間。単位はmilliseconds。
4. `time_to_debug`: コーダーがデバッグをするのにかかる時間。単位はmilliseconds。
5. `time_to_refactor`: コーダーがリファクタリングするのにかかる時間。単位はmilliseconds。
6. `number_of_compiles_required`: 全てのコーダーが少なくとも`number_of_compiles_required`回コンパイルすると、プトグラムは終了する。
7. `dongle_cooldown`: ドングルのクールダウンにかかる時間。単位はmilliseconds。
8. `scheduler` : fifoあるいはedfが入る。複数のコーダーが同時に同じドングルを取りたい状況になった際のルールの指定。  
fifo: 最も早くドングルを取れる状態になっていたコーダーが優先。  
edf: burnoutまでの時間が最も短いコーダーが優先。

それぞれのコーダーは1から`number_of_coders`までのIDを持ちます。IDが１番のコーダーはIDが`number_of_coders`番のコーダーの隣に座ります。IDがN番のコーダーはN-1番とN + 1番のコーダーの間に座ります。  
ログは以下のような形式で標準出力に出力されます。
```bash
# <時間>　<ID> <動作>
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
以上のような制約のもとで、コーダーの動くを順番に出力していくプログラムを作成します。

## Instruction(Usage)
リモートリポジトリからクローンします。
```bash
git clone https://github.com/takutotakuto1121-creator/Codexion.git codexion
```
リポジトリに移動します。
```bash
cd codexion
```
以下のコマンドでコンパイルし、実行可能なファイル`codexion`を作成します。
```bash
make
```
実行します。引数は左から順番にDescription/詳細セクションの1~8の情報に対応します。これらは自由に変更できます。以下の値は例です。
```bash
./codexion 4 800 200 200 200 5 10 fifo
```

## Blocking cases handled
デッドロックの対策方法を記述する。この部分がこの課題の本質である。例えば、全コーダーが同時に左のドングルを確保した状態で、右のドングルの空きを待機する状況になったとします。ここから状況は動くことがなく無限ループにハマってしまいます。これをデッドロックといいます。デッドロックの解消方法は調べればいくらでも出てきますが、今回はIDが`number_of_coders`番のコーダーだけは右のドングルを先に確保し、左のドングルを確保するようにし、それ以外のコーダーは左にドングルを先に確保するという実装にすることにより解決しました。

## Thread synchronization mechanisms
スレッドはメインのスレッドに追加で、コーダー各1人につき一つ、モニターで１つ追加で使用しています。
### コーダーのスレッド
以下のループを繰り返しています。
1. ドングルの確保。確保できるドングルがない場合はできるようになるまで待機。
2. コンパイルし、ドングルを手放す。
3. デバッグし、リファクタリングをする。
4. もし、終了条件を満たしていれば、スレッドを終了する。満たしていなければ1に戻る。

### モニターのスレッド
以下のループを繰り返しています。
1. バーンアウトの基準を満たすコーダーがいれば、検知し終了の旨をコーダーのスレッドに送り、自分も終了する。
2. 全てのコーダーが少なくとも`number_of_compiles_required`回コンパイルしていれば、検知し終了の旨をコーダーのスレッドに送り、自分も終了する。そうでなければ、1に戻る。

## Resources
### 使用した技術
pthredライブラリを使用しています。[memo](memo.md)に列挙されているような関数を使用しています。メモなので可読性は勘弁。
### AIの使用
コードの基盤は全て手書きで実装しています。AIは使用していません。   
norminette(42のコーディング規約)エラーの解消にはAIを使用しました。Milestone3の課題なので、この部分は本質ではないからです。また、README.mdの日本語verは手書きで記述しましたが、English ver.はAIに日本語ver.を翻訳させることにより作成しました。

### 参考文献
[C言語でのマルチスレッドの解説](https://daeudaeu.com/multithread/).  
[剰余演算により配列に循環的にアクセスする方法](https://www.cc.kyoto-su.ac.jp/~yamada/ap/cyclicIndex.html).  

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
This section describes the strategy to prevent deadlocks, which is the core challenge of this project. For instance, imagine a scenario where all coders simultaneously acquire their left dongle and wait for their right dongle to become available. The situation will not progress, resulting in an infinite loop. This is known as a deadlock. There are countless ways to resolve deadlocks if you look them up, but for this project, I resolved it by implementing a rule where only the coder with the ID `number_of_coders` acquires their right dongle first, followed by the left dongle. All other coders acquire their left dongle first.

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













