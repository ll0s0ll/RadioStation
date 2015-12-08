/*
radioStation.c

Copyright (C) 2015 Shun ITO <movingentity@gmail.com>
      
This file is part of RadioStation.
RadioStation is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or any later version.

RadioStation is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RadioStation.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUF_SIZE 512
#define MAX_CMD_SIZE 1024


int checkStatus(int status)
{  
  if (status == -1) {
    fprintf(stderr, "ERROR:子プロセスを作成できなかった or 終了ステータスを取得できなかった。\n");
    return -1;
  } else if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
    fprintf(stderr, "ERROR:(たぶん) シェルが実行できなかった。\n");
    return -1;
  } else if (WIFSIGNALED(status)) {
    //fprintf(stdout, "シグナル %d (%s)によってkillされた。\n", WTERMSIG(status),strsignal(WTERMSIG(status)));
    return -1;
  } else if (WIFSTOPPED(status)) {    
    //fprintf(stdout, "シグナル %d (%s)によって停止された。\n", WSTOPSIG(status),strsignal(WSTOPSIG(status)));
    return -1;
  }

  return 0;
}


int makeWav(char *path_aqTalkPi, char *path_wavfile, char *text)
{
  // system()で実行するコマンド文字列を作成。
  // 標準出力は出力しないように、/dev/nullにまわす。
  // AquesTalk Piは、16bit 8000Hz MONOのWAVを作成する。
  char cmd[MAX_CMD_SIZE];
  sprintf(cmd, "%s -o %s \"%s\" > /dev/null", path_aqTalkPi, path_wavfile, text);
  //printf("cmd:%s\n", cmd);

  // コマンドを実行(標準出力は出力しない)
  int status = system(cmd);

  return checkStatus(status);
}


int playWav(char *path_pifm, float radio_freq, char *path_wavfile, int wav_freq)
{
  // system()で実行するコマンド文字列を作成。
  // PifmはGPIOを使用するため、管理者権限で実行する必要がある。
  // 標準出力は出力しないように、/dev/nullにまわす。
  char cmd[MAX_CMD_SIZE];
  sprintf(cmd, "sudo %s %s %f %d > /dev/null", path_pifm, path_wavfile, radio_freq, wav_freq);
  //printf("cmd:%s\n", cmd);

  // コマンドを実行
  int status = system(cmd);

  return checkStatus(status);
}

void printHelp()
{
  printf("\
NAME\n\
\tRadioStation\n\
\n\
SYNOPSIS\n\
\tradioStation text.txt [-a Path to AquesTalk Pi] [-f Radio Freqency] [-h] [-p Path to pifm] [-w Path to Temp WAV file]\n\
\n\
DESCRIPTION\n\
\tなんちゃってFMラジオ放送局プログラムです。\n\
\tRaspberry Pi用のラジオトランスミッタプログラム'pifm'(http://razzpisampler.oreilly.com/ch01.html)、\n\
\t同じくRaspberry Pi用の音声合成プログラム'AquesTalk Pi'(http://www.a-quest.com/products/aquestalkpi.html)を使用します。\n\
\t指定されたテキストファイルを読み込み、テキストを音声に変換し、音声を電波に変換しています。\n\
\tテキストファイルは文末まで読み上げると、先頭に戻り、繰り返し読み上げます。\n\
\t付属のsample.txtでは、プログラムを開始すると、ジングルが流れ、交通情報のようなものが読み上げられます。\n\
\n\
OPTIONS\n\
\ttext.txt\n\
\t発話する文書を記載したテキストファイルをのパスを指定します。(UTF-8)。\n\
\t-a\n\
\tAquesTalk Piへのパスを指定します。指定がない場合、radioStationと同じディレクトリにあるものとして実行します。\n\
\t-f\n\
\tラジオの周波数を指定します。指定がない場合、89.9MHzで実行します。\n\
\t-h\n\
\tこのメッセージを表示します。\n\
\t-p\n\
\tpifmへのパスを指定します。指定がない場合、radioStationと同じディレクトリにあるものとして実行します。\n\
\t-w\n\
\t一時的に作成する、文書を音声に変換したWAVファイルのパスを指定します。\n\
\t指定がない場合、'/tmp'ディレクトリ下に'radioStation.wav'として作成します。\n\
\n\
EXAMPLE\n\
\t$ ./radioStation sample.txt\n\
\t$ ./radioStation sample.txt -a ~/aquestalkpi/AquesTalkPi -f 77.5 -p ~/pifm/pifm -w ~/tmp.wav\n\
\n\
");
}


int main(int argc, char **argv)
{

  if (argc < 2) {
    printHelp();
    exit(EXIT_SUCCESS);
  }


  char *path_aqTalkPi = NULL;
  char *path_pifm = NULL;
  char *path_textfile = NULL;
  char *path_wavfile = NULL;
  float radio_freq = 0.0;


  // 引数解析
  int opt;
  while ((opt = getopt(argc, argv, "a:f:hp:w:")) != -1) {

    switch (opt) {
    case 'a':
      // AquesTalk Piへのパス
      path_aqTalkPi = optarg;
      break;
      
    case 'f':
      // 放送するラジオの周波数
      radio_freq = atof(optarg);
      break;

    case 'h':
      // ヘルプを表示
      printHelp();
      exit(EXIT_SUCCESS);
      break;
      
    case 'p':
      // pifmへのパス
      path_pifm = optarg;
      break;
      
    case 'w':
      // pifmへのパス
      path_wavfile = optarg;
      break;
      
    case '?':
      fprintf(stderr, "ERROR:オプションが不明です。\n");
      printHelp();
 
    default: exit(EXIT_FAILURE);
    }    
  }

  // オプション以外の引数に対する処理
  if (optind < argc) {
    path_textfile = argv[optind++];
  }

  // 読み上げるテキストへのパス
  if (path_textfile == NULL) { 
    fprintf(stderr, "ERROR:読み上げるテキストファイルが指定されていません。\n");
    exit(EXIT_FAILURE);
  }

  // AquesTalk Piプログラムへのパス。
  // 値が指定されていない場合、デフォルト値を設定。
  if (path_aqTalkPi == NULL) {
    path_aqTalkPi = "./AquesTalkPi";
  }

  // pifmプログラムへのパス
  // 値が指定されていない場合、デフォルト値を設定。
  if (path_pifm == NULL) {
    path_pifm = "./pifm";
  }

  // テキストを音声に変換したWAVファイルのパス
  // 値が指定されていない場合、デフォルト値を設定。
  if (path_wavfile == NULL) {
    path_wavfile = "/tmp/radioStation.wav";
  }

  // ラジオの周波数
  // 値が指定されていない場合、デフォルト値(89.9MHz)を設定。
  if (radio_freq == 0.0) {
    radio_freq = 89.9;
  }


  // テキストファイルを開く
  FILE *fp;
  char buf[MAX_BUF_SIZE] = {'\0'};
  if ((fp = fopen(path_textfile, "r")) == NULL) {
    fprintf(stderr, "ERROR:テキストファイルが開けませんでした。\n");
    exit(EXIT_FAILURE);
  }


  // エラーが出ない限り永遠に処理を続ける。
  int ret = 0;
  while (ret == 0) {

    // ジングルを再生。付属のジングルは、16bit 8000Hz MONO形式。
    ret = playWav(path_pifm, radio_freq, "jingle.wav", 8000);
    if (ret != 0) { break; }

    // ファイルの終端まで文字を読み取り表示する
    while ( fgets(buf, MAX_BUF_SIZE, fp) != NULL ) {

      // 改行除去
      strtok(buf, "\n\0");
      //printf("%s\n", buf);

      // AquesTalk Piを使い、テキストを音声に変換したWAVファイルを作成。
      ret = makeWav(path_aqTalkPi, path_wavfile, buf);
      if (ret != 0) { break; }

      // AquesTalk Piは、16bit 8000Hz MONOのWAVを作成する。
      ret = playWav(path_pifm, radio_freq, path_wavfile, 8000);
      if (ret != 0) { break; }

    }

    // 先頭まで巻き戻す。
    rewind(fp);
  }

  // テキストファイルを閉じる
  fclose(fp);

  // 一時WAVファイルを削除
  remove(path_wavfile);

  return 0;
}
