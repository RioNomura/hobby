#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**********
 * マクロ *
 **********/
#define MAX_EMPLOYEE (8) /* 最大従業員数 */
#define DAYS_IN_MONTH (31) /* １ヶ月の最大の日数 */
#define CUT_OFF_DATE (15) /* 給与締め日 */
#define PAYMENT_DATE (25) /* 給与支払い日 */
#define INVALID (-1) /* 時刻データ無効 */
#define MANAGER (0) /* 管理者 */
#define MAX_STRING (32)
#define PAYMENT_BY_HOUR (1000) /* 時給 */
#define PAY_SLIP_FILE "payslip_"

/* コマンド */
#define START_TIME (1) /* 始業時刻の登録 */
#define END_TIME (2) /* 退社時刻の登録 */
#define EMPLOYEE (3) /* 従業員ごとの作業時間 */
#define DATE (4) /* 日付ごとの作業時間 */

/* 就業状態 */
#define ENGAGING 'e' /* 就業中 */
#define NON_ENGAGING 'n' /* 退社済 */
#define VACANT '\0' /* 非雇用(この番号の従業員は雇用していない) */

/**********
 * 型宣言 *
 **********/
typedef struct {
  int id; /* 従業員番号 */
  int start_hour; /* 出社の時間 */
  int start_minute; /* 出社の分 */
  int end_hour; /* 退社の時間 */
  int end_minute; /* 退社の分 */
  double *working; /* 作業時間へのポインタ */
  char *engagement; /* 就業状態へのポインタ */
} worker;

/********************
 * プロトタイプ宣言 *
 ********************/
int get_command(void);
int get_today(char *);
int get_time(void);
worker *get_employee(worker[MAX_EMPLOYEE]);
int record_start(worker[MAX_EMPLOYEE]);
int record_end(worker[MAX_EMPLOYEE]);
int initialize_timeData(worker *);
int associate_workers(worker[MAX_EMPLOYEE], double[MAX_EMPLOYEE][DAYS_IN_MONTH], char[MAX_EMPLOYEE]);
int load_timeData(double[MAX_EMPLOYEE][DAYS_IN_MONTH],char[MAX_EMPLOYEE]);
int save_timeData(double[MAX_EMPLOYEE][DAYS_IN_MONTH], char[MAX_EMPLOYEE]);
double calculate_workingTime(int, worker *);
int sumUp_employeeWorking(worker[MAX_EMPLOYEE]);
int sumUp_dateWorking(worker[MAX_EMPLOYEE]);
int create_paySlip(worker *);
int is_firstDayAfterCutoff(int);
int is_firstDayAfterPayment(int);
int count_engaging(worker[MAX_EMPLOYEE]);

/********
 * 関数 *
 ********/
int
main(int argc,
     char *argv[])
{
  int date; /* 今日の日付(yyyymmddのddのみ) */
  int employee; /* 従業員番号 */
  int command; /* タイムレコーダへのコマンド */
  worker workers[MAX_EMPLOYEE]; /* 従業員全員のデータ */
  worker *person; /* workersへのポインタ */

  /* この部分は終了時にファイルに保存 */
  int paySlip; /* 給与明細を作成したことをしめすフラグ(作成していれば1) */
  double working[MAX_EMPLOYEE][DAYS_IN_MONTH] = {{0.0}}; /* 作業時間
							    すべて0.0で初期化 */
  char engagement[MAX_EMPLOYEE]; /* 各従業員が就業中かの就業状態 */



  /* 前日までのデータをファイルからロード */
  load_timeData(working, engagement); /* データロードは本プログラム起動時に実施 */
  paySlip = 0; /* 起動時には payslipを"未作成"に初期化 */
  
  /* 配列working，配列engagementと workersの中のメンバとの対応 */
  associate_workers(workers, working, engagement);

  if(argc != 2) {
    printf("Usage: %s yyyymmdd\n", argv[0]);
    return INVALID;
  } else  
    if((date = get_today(argv[1])) == INVALID) { /* 今日の日付を取得する. */
      printf("Usage: %s yyyymmdd\n", argv[0]);
      return INVALID;
    }
      
  /* 給与明細の作成制御 */
  /* 　給与締め日以降の最初の出勤日に給与明細を作成 */
  /* 　給与支払い日以降の最初の出勤日に，給与締め日になれば次月の給与明細を作成すべきことを示す */
  if(is_firstDayAfterCutoff(date) && (! paySlip)) { /* 給与締め日以降の最初の出勤日か */
    /* 全員分の給与明細作成 */
    for(person = workers, employee = 0; employee < MAX_EMPLOYEE; employee++, person++) {
      if(*(person->engagement) == VACANT)
	continue;
      if(create_paySlip(person) == INVALID)
	return INVALID;
      else {
	/* 給与計算した従業員の作業データを初期化 */
	initialize_timeData(person);
      }
      paySlip = 1; /* 給与明細を作ったことを示す */
    }
  } else if(is_firstDayAfterPayment(date)) /* 給与支払い日以降の最初の出勤日か */
    paySlip = 0; /* 次の月の給与明細を未作成であることを示す */
  
  do {
    printf("** タイムレコーダ ** \n ");
    if((person = get_employee(workers)) == NULL) /* 従業員データ */
      continue;
    
    switch(command = get_command()) {  /* コマンドの取得 */
    case START_TIME:
      if(*(person->engagement) != NON_ENGAGING) {
	printf("従業員%dはすでに就業しています\n", person->id);
	continue;
      } else 
	record_start(person);
      break;
    case END_TIME:
      if(*(person->engagement) != ENGAGING) {
	printf("従業員%dはすでに退社しています\n", person->id);
	continue;
      } else {
	record_end(person);
	/* 作業時間を時間単位で計算 */
	calculate_workingTime(date, person);
      }
      break;
    case EMPLOYEE:
    case DATE:
      if(person->id != MANAGER) {
	printf("この機能は管理者だけが使えます\n");
      } else {
	if(*(person->engagement) != ENGAGING) {
	  printf("従業員%dは出勤時刻を登録してください\n", MANAGER);
	  record_start(person);
	}
	if(command == EMPLOYEE)
	  sumUp_employeeWorking(workers); /* 従業員ごとの作業時間表示 */
	else
	  sumUp_dateWorking(workers); /* 日ごとの作業時間表示 */
      }
      break;
    default:
      printf("正しい操作を指定してください\n");
      break;
    }
  } while( count_engaging(workers) > 0 ); /* 従業員が全員退社すればループを抜ける */

  save_timeData(working, engagement);
  
  return 0;
}

/****************
 * コマンド取得 *
 ****************/
int
get_command(void)
{
  int command;
  char string[MAX_STRING];
  
  printf("コマンド[");
  printf("%d(出勤) ", START_TIME);
  printf("%d(退社) ", END_TIME);
  printf("%d(従業員毎就業時間) ", EMPLOYEE);
  printf("%d(日毎就業時間)] ", DATE);

  fgets(string, MAX_STRING-1, stdin); 
  command = atoi(string); /* 整数に変換 */
  
  return command;
}

/********************
 * 就業中の従業員数 *
 ********************/
int
count_engaging(worker workers[MAX_EMPLOYEE])
{
  worker *person;
  int i;
  int number = 0; /* 就業中の従業員数 */

  for(i = 0, person = workers; i < MAX_EMPLOYEE; i++, person++) {
    if(*(person->engagement) == ENGAGING)
      number++;
  }
  return number;
}

/**************************
 * その日の労働時間を計算 *
 **************************/
double
calculate_workingTime(int date, worker *person)
{
  double hour;
  int minute;

  minute = (person->end_hour - person->start_hour) * 60;  /* 分単位に変換 */
  minute = minute + (person->end_minute - person->start_minute);

  hour = minute / 60.0; /* 時間単位に変換 */
  *(person->working + date) = hour; /* これが正解 */
  	/* ただし，ソースコードの読みやすさからすると修正する前のほうがわかりやすい */
  return hour;
}

/******************************
 * 従業員ごとの労働時間を計算 *
 ******************************/
int
sumUp_employeeWorking(worker workers[MAX_EMPLOYEE])
{
  worker *person;
  int employee;
  int date;
  double time;

  printf("従業員\t勤務時間合計\n");
  for(employee = 0, person = workers; employee < MAX_EMPLOYEE; employee++, person++) {
    time = 0.0;
    for(date = 0; date < DAYS_IN_MONTH; date++)
      time += person->working[date];
    printf("%4d\t%10f\n", employee, time);
  }
  puts("");
  return 0;
}

/******************************
 * 日ごとの労働時間を計算 *
 ******************************/
int
sumUp_dateWorking(worker workers[MAX_EMPLOYEE])
{
  worker *person;
  int employee;
  int date;
  double time;

  printf("日付\t勤務時間合計\n");
  for(date = 0; date < DAYS_IN_MONTH; date++) {
    time = 0.0;
    for(employee = 0, person = workers; employee < MAX_EMPLOYEE; employee++, person++) {
      time += person->working[date];
    }
    printf("%4d\t%10f\n", date+1, time);
  }
  puts("");
  return 0;
}

/*****************************************
 * 従業員の給与明細を計算・CSV形式で作成 *
 *****************************************/
int create_paySlip(worker *person)
{
  /* いまは何もしない */
  
  return 0;
}



/********************
 * 従業員データ取得 *
 ********************/
worker *get_employee(worker workers[MAX_EMPLOYEE])
{
  int employee;
  char string[MAX_STRING];
  worker *person;

  do {
    printf("従業員番号: ");
    fgets(string, MAX_STRING-1, stdin);
    employee = atoi(string); /* 整数に変換 */

    if((employee < 0) || (employee>= MAX_EMPLOYEE)) {
      printf("正しい従業員番号を指定してください\n");
      continue;
    } else {
      person = workers+employee;
      if(*(person->engagement)==VACANT) {
	printf("従業員%dは雇用されていません\n", person->id);
	person = (worker *)NULL; /* 雇用していなければNULLを返す */
      }
      return person;
    }
  } while(1);
}


/****************************
 * 日付取得(1日ならdateは0) *
 ****************************/
int
get_today(char *string)
{
  int full; /* 年月日(yyyymmdd) */
  int date; /* 日のみ */
  int month; /* 月のみ */

  if(strlen(string) != strlen("yyyymmdd")) { /* 長さ比較 */
    printf("正しい日付を指定してください\n");
    return INVALID;
  }

  full = atoi(string); /* 整数に変換 */

  date = full % 100;
  month = (full / 100) % 100;

  if((date < 1) || (date> 31)) {
    printf("正しい日付を指定してください\n");
    return INVALID;
  } else if((month < 1) || (month > 12)) {
    printf("正しい日付を指定してください\n");
    return INVALID;
  } else
    return date - 1; /* 1日なら0 */
}


/************
 * 時刻取得 *
 ************/
int get_time(void)
{
  char string[MAX_STRING];
  int time; /* hhmm形式の時刻(24時間60分表記) */
  int hour;
  int minute;

  do {
    printf("(hhmm): ");
    fgets(string, MAX_STRING-1, stdin);
    if(strlen(string) -1 != strlen("hhmm")) { /* fgets()では\nのぶんだけ1文字多い */
      printf("正しい時間を指定してください\n");
      continue;
    }

    time = atoi(string); /* 整数に変換 */
    hour = time /100; /* 時間パート抽出 */
    minute = time % 100; /* 分パート抽出 */

    if((hour < 0) || (hour> 23)) {
      printf("正しい時間を指定してください\n");
    } else if((minute < 0) || (minute> 59)) {
      printf("正しい分を指定してください\n");
    } else
      return time;
  } while(1);
}
  

/****************
 * 出勤時刻取得 *
 ****************/
int
record_start(worker *person)
{
  int time;

  printf("出勤時刻");
  time = get_time();
  person->start_hour = time / 100;
  person->start_minute = time % 100;
  *(person->engagement) = ENGAGING; /* 就業中へ */

  puts("");

  return ENGAGING;
}

/****************
 * 退社時刻取得 *
 ****************/
int record_end(worker *person)
{
  int time;

  printf("退社時刻");
  time = get_time();
  person->end_hour = time / 100;
  person->end_minute = time % 100;
  *(person->engagement) = NON_ENGAGING; /* 非就業へ */
  puts("");

  return NON_ENGAGING;
}

/****************************
 * 締め日後の最初の出勤日か *
 ****************************/
int
is_firstDayAfterCutoff(int date)
{
  if(date+1 > CUT_OFF_DATE)
    return 1;
  else
    return 0;
}

/****************************
 * 給料日後の最初の出勤日か *
 ****************************/
int
is_firstDayAfterPayment(int date)
{
  if(date+1 > PAYMENT_DATE)
    return 1;
  else
    return 0;
}

/************************************
 * 任意の従業員の時間データの初期化 *
 ************************************/
/* 雇用していない従業員については初期化しない */
int
initialize_timeData(worker *person) /* 引数をpersonにしたので，*/
{ /*start_hour, start_minute, end_hour, end_minuteの初期化もこの関数で実施  */
  int date;

  for(date = 0; date < DAYS_IN_MONTH; date++) {
    person->working[date] = 0.0;
  }
  person->start_hour = INVALID; /* 無効に設定 */ 
  person->start_minute = INVALID; /* 無効に設定 */ 
  person->end_hour = INVALID; /* 無効に設定 */ 
  person->end_minute = INVALID; /* 無効に設定 */ 

  return 0;
}

/*******************************************
 * working，engagementと workersの対応づけ *
 *******************************************/
int
associate_workers(worker workers[MAX_EMPLOYEE],
		  double working[MAX_EMPLOYEE][DAYS_IN_MONTH],
		  char engagement[MAX_EMPLOYEE])
{
  worker *person;
  int employee;
  
  person = workers;
  for(employee = 0; employee < MAX_EMPLOYEE; employee++) {
    person->id = employee;
    person->working = working[employee]; /* その従業員の毎日のworkingデータの先頭アドレス */
    person->engagement = engagement + employee; /* その従業員のその日の就業状態のアドレス */
    person++;
  }
  return 0;
}

/**********************
 * 時間データのロード *
 **********************/
int
load_timeData(double working[MAX_EMPLOYEE][DAYS_IN_MONTH],
	      char engagement[MAX_EMPLOYEE])
{
  int employee;
  int date;

  /* メモリをクリア */
  for(employee = 0; employee < MAX_EMPLOYEE; employee++) {
    for(date = 0; date < DAYS_IN_MONTH; date++) {
      working[employee][date] = 0.0;
    }
    engagement[employee] = VACANT;
  }
  
  /* 従業員0(管理者)は 1日（配列上は0）に */
  working[0][0] = 11.5; /* これだけ働いた */
  engagement[0] = NON_ENGAGING;

  /* 従業員1は1日（配列上は0）に */
  working[1][0] = 8.5; /* これだけ働いた */
  engagement[1] = NON_ENGAGING;
  
  /* 従業員2は1日（配列上は0）に */
  working[2][0] = 8.5; /* これだけ働いた */
  engagement[2] = NON_ENGAGING;
  
  /* 従業員3は1日（配列上は0）に */
  working[3][0] = 10.2; /* これだけ働いた */
  engagement[3] = NON_ENGAGING;

  return 0;
}

/**********************
 * 時間データのセーブ *
 **********************/
int
save_timeData(double working[MAX_EMPLOYEE][DAYS_IN_MONTH],
	      char engagement[MAX_EMPLOYEE])
{
  /* 現時点では，何もしない */
  return 0;
}

