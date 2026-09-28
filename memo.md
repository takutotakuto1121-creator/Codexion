
## コア数とスレッド数の確認
```bash
sysctl hw.physicalcpu hw.logicalcpu
```
出てきたスレッド数をNUM_THREADに定義する。

## 実行時間の計算
### gettimeofday()
システムの現在時刻を100万分の1秒単位で取得できる関数
```C
int gettimeofday(struct timeval *tv, struct timezone *tz)
```
tv: timeval構造体へのポインタ
tz: 現在非推奨。NULLを常に渡せばOK

### timeval
```C
struct timaval{
    time_t      tv_sec;     // 秒
    suseconds_t tv_usec;    // マイクロ秒
}
```

#### 使用方法
```C
struct timeval  start;
struct timeval  end;
double          time;

gettimeofday(&start, NULL); // 開始時刻の記録
// 計測したい処理の記述
gettimeofday(&end, NULL);   // 終了時刻の記録

time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)  // 実行時間の計算(終了 - 開始)
```

## マルチスレッド
### pthread_create()
新しいスレッドを作成し、指定した関数の実行を並行して開始する関数
```C
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void  *(*start_routine)(void *), void *arg)
```
thread: 作成されたスレッドのIDがここに書き込まれる
attr: スレッドの属性設定(デフォルトで動かす場合はNULL)
start_routine: スレッドに実行させる関数へのポインタ(void *関数名(void *))
arg: スレッドの関数に渡す引数(複数の情報を渡したい場合は、構造体にまとめる)

### pthread_join()
指定したスレッドの処理が完全に終了するまで、一時停止させて待機する関数
```C
int pthread_join(pthread_t thread, void **retval)
```
thread: 終了を待ちたいスレッドのID
retval: スレッド関数からの戻り値を受け取るポインタ(不要な場合はNULL)

### pthread_t
スレッドを識別するためのID番号を保持するデータ型
```C
pthread_t   thread_id
```

### 使用方法
```C
pthread_t   t;
struct data d;

d.start = 0;
d.num = 20;

pthread_create(&t, NULL, func, &d);

pthread_join(t, NULL);
// これを書かないとスレッドが終わる前にmain()が終了してしまう
```

## mutex
### pthread_mutex_t
mutex(ロック)の状態を保持するためのデータ型
```C
pthread_mutex_t mutex;
```

### pthread_mutex_init()
mutexを初期化する関数
```C
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
```
mutex: 初期化するmutex変数へのポインタ
attr: mutexの属性設定(デフォルトの場合はNULL)

### pthread_mutex_lock()
mutexをlockする関数。すでに他のスレッドがロックしている場合は、ロックが解除されるまで待機。
```C
int pthread_mutex_lock(pthread_mutex_t *mutex);
```
mutex: lockするmutex変数へのポインタ

### pthread_mutex_unlock()
mutexのlockを解除する関数。待機中の他のスレッドがあれば、そのうちの１つがロックを取得して処理を再開する。
```C
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```
mutex: lockを解除するmutex変数へのポインタ

### pthread_mutex_destroy()
使用済みのmutexを破棄し。システムリソースを解放する関数
```C
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
mutex: 破棄するmutex変数へのポインタ

### 使用方法
```C
pthrad_mutex_t  mutex;
int             count = 0;

void    *func(void  *arg)
{
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(void)
{
    pthread_t   t1, t2;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&t1, NULL, func, NULL);
    pthread_create(&t2, NULL, func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_mutex_destroy(&mutex);
    return (0);
}
```

## cond
### pthread_cond_t
条件変数の状態を保持するためのデータ型
```C
pthread_cond_t  cond;
```

### pthread_cond_init()
条件変数を初期化する関数
```C
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
```
cond: 初期化する条件変数へのポインタ
attr: 属性設定(デフォルトの場合はNULL)

### pthread_cond_wait()
条件が満たされるまでスレッドを一時停止させる関数。   
この関数を呼ぶと、待っているmutexのロックを自動で一旦解除し、スリープ状態に入るという動作をします。その後後述のシグナルを受け取って目覚める瞬間にもう一度mutexをロックしてから次の行に進みます。
```C
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
```
cond: 待機する条件変数へのポインタ
mutex: 関連づけられたmutexへのポインタ

### pthread_cond_signal()
待機中のスレッドに、準備ができたとシグナルを送り、待機中のスレッドのうち１つのスレッドを起こす関数。
```C
int pthread_cond_signal(pthread_cond_t *cond);
```
cond: 通知を送る条件変数へのポインタ

### pthread_cond_broadcast()
待機中のスレッドに、準備ができたとシグナルを送り、待機中の全てスレッドを起こす関数。
```C
int pthread_cond_broadcast(pthread_cond_t *cond);
```
cond: 条件変数へのポインタ

### pthread_cond_destroy()
使用済みの条件変数を破棄し、システムリソースを解放
```C
int pthread_cond_destroy(pthread_cond_t *cond);
```
cond: 条件変数へのポインタ



## 設計メモ
### マクロs
1. NUM_THREAD
2. ERROR

### structures
1. t_args
2. t_coder

### パース & バリデーション
まあ普通に

### monitorスレッド
whileで常に監視
1. burnoutテェック: 全こーだーが最後にコンパイルした時間を見て周り、現在時刻との差が`time_to_born_out`を超えていないかを確認

### codersスレッド

### デッドロック

## 剰余演算による配列の循環
配列の要素数をNとすると、インデックスにindex % Nを指定する。
すると、インデックスが0 ~ N-1 の範囲を超えても循環してくれる。

### ちょっと全体像が掴めん
最小限から進化させてく
1. monitorなし、coder１人で実装
2. coderを複数人に
3. monitorの追加
4. fifo, edf
5. 諸々最終調整



