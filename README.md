# kernelhack
http://www.coins.tsukuba.ac.jp/~yas/coins/slab-kernel-2015/
これは私的なリポジトリのため後輩の役には立ちません。

## 課題1 カーネルのコンパイルとパラメタの設定
files/config\_template や tools/kerninst.sh を参照のこと
## 課題2 カーネルのリモート・デバッグ
## 課題3 システムコールの追加
syscall/ にカーネル空間でのシステムコール実装とそれを呼び出すユーザー空間のプログラムがある
## 課題4 デバイス・ドライバの作成
driver/
## 課題9 その他 (VPNをつくりました)
TUNデバイスを使ってL3のVPNをしてみる

* myvpn.c
  * peer to peerな対等なVPN。サーバもクライアントもお互いに固定ポート先に送り合う。
* myvpn_nat.c
  * クライアント側にあるfirewallやNATに邪魔されないよう、サーバはクライアントのソースポートに返事をするようにした。
  * サーバはクライアントから最初の1パケットを見てからはじめてクライアントのポートを知るので、かならずクライアントが何かパケットを送ってこないとサーバからクライアントへは届けられない。

詳細は以下のREADME.md
( https://github.com/keiya/kernelhack/tree/master/myvpn )
