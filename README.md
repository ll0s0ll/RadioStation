# RadioStation – Raspberry Piで、なんちゃってFMラジオ放送局を作る
ある日、[Raspberry Piクックブック](http://www.oreilly.co.jp/books/9784873116907/)を読んでいたら、「Raspberry PiをFMトランスミッターにする」という記事が載っていました。これは面白いと思って試してみると、ラジオからあのスターウォーズの音楽が。これはやっぱり面白い！他にも何か使えるのでは？と思ったのが始まりです。

そこでなんとなく思いついたのが、ハイウェイラジオ。高速道路を走っていると、よく、ここからハイウェイラジオ1620kHz、なんて標識が出ているあれです。[録音の動画](https://youtu.be/idRgEm3oFnc)がありましたので、参考にさせていただきました。

動作を収録した動画を撮りました。

https://user-images.githubusercontent.com/3046839/210188551-7453825d-b204-4127-86de-c839e830c471.mp4

また、真面目なところだと、災害等発生時に、仮設避難所などで、行政の情報や、交通情報、支援情報などを、内容を自動更新にして流せば、何かの役に立つかもしれません。災害発生時には、ラジオが情報源として役に立ったときいています。

※書籍の補足にもありましたが、**不法な電波の発信は電波法等の法令に違反する可能性があります**。通常の使い方では心配ないそうですが、あまりに長い電線や大きなアンテナは接続しないようにしてください。机の上でテストする分には、まったく電線を付けなくても十分聞こえました。

## 概要
指定されたテキストファイルを一行づつ読み込み、テキストを音声に変換し、音声を電波に変換しています。テキストファイルは文末まで読み上げると、先頭に戻り、繰り返し読み上げます。付属のsample.txtでは、プログラムを開始すると、ジングルが流れ、交通情報のようなものが読み上げられます。

## 必要なもの
- pifm  
  Raspberry PiをFMトランスミッタにしてしまうプログラム

- AquesTalk Pi  
  [株式会社アクエスト](http://www.a-quest.com/)から公開されている音声合成プログラム。個人が非営利の目的に使う場合は無償で使える(感謝!!)。


## 使い方
1. pifmをダウンロード及び展開します。（ダウンロードはこちらから。）展開先のパスが必要となりますので、控えておいてください。

1. AquesTalk Piをダウンロード及び展開します。（ダウンロードはこちらから。展開方法を説明したブログへのリンクも張られています。）展開先のパスが必要となりますので、控えておいてください。

1. RadioStationをダウンロード及び展開します。（ダウンロードはこちらから。）以下は例。
    ```sh
    $ mkdir radioStation
    $ cd radioStation
    $ wget https://dl.dropboxusercontent.com/u/260932024/radioStation.tar
    $ tar -xf radioStation.tar
    ```

1. RadioStationを実行します。オプション-aでAquesTalk Piへのパス、-fでラジオの周波数、-pでpifmへのパスを指定します。以下は例。
    ```sh
    $ ./radioStation sample.txt -a ~/aquestalkpi/AquesTalkPi -f 77.5 -p ~/pifm/pifm
    ```

1. RadioStationを終了する。ctrl-c等で終了させてください。

1. ヘルプを表示する。オプション-hを付けて実行してください。
    ```sh
    $ ./radioStation -h
    ```
