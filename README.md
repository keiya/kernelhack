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

### つかいかた(myvpn.c)
myvpn.cはサーバー・クライアントの概念はありません。Peer to Peerな関係です。
通信は一対一で、相互とも相手を指定する必要があります。また、NAT環境下などでは利用できません。
どちらからもUDPが届く必要があります。


ピアA
./myvpn -a 172.16.5.129 -p 12345 -t "ifconfig %s 192.168.1.1 mtu 1450  pointopoint 192.168.1.2"
ピアB
./myvpn -a 172.16.5.1 -p 12345 -t "ifconfig %s 192.168.1.2 mtu 1450 pointopoint 192.168.1.1" 

-aが相手のIPアドレス、pが接続先ポートです。申し訳ないがリッスンポート12345はハードコードされていて引数からのインターフェイスはない。
このプログラムを実行するとPeer-to-peerなTUNが作成されます。tunのMTUは1450で、理由は、標準的な1500にしてしまうと、それに対してUDPヘッダをエンカプセレーションで付けてしまって1500以上になり、1つの元のパケットが2つに別れてしまうフラグメントが発生します。
そうなるとオーバーヘッドになるので、tunを最初から1450にしてそういうことを防止しています。
### つかいかた(myvpn_nat.c)
サーバ

./myvpn_nat -s 12345 -t "ifconfig %s 192.168.1.2 mtu 1450 pointopoint 192.168.1.1"
* -s バインドポート（このUDPポートでリッスン）
* -t ifconfig 設定文字列。
  * MTUはLAN内なら1450くらいでOK（詳しく調べてない）
  * 筑波大学→さくらVPSは1422

クライアント
./myvpn_nat -a 160.16.95.238 -p 12345 -t "ifconfig %s 192.168.1.1 mtu 1450  pointopoint 192.168.1.2"
* -a サーバのIPアドレス
* -p サーバーのポート

### サーバをデフォルトゲートウェイ化
#### クライアント側
VPNサーバへのmyvpnパケット以外全部tun0(192.168.1.2)に流します。

sudo route add -host [VPNサーバのIPアドレス] gw [いつも使ってるデフォルトゲートウェイ] dev [eth0などデバイス名]
sudo route add default gw 192.168.1.2

#### サーバー側
サーバのtunに到着したパケットをeth0から出るようにしてNAT（ipマスカレード）するようにします。


iptables -I FORWARD -i tun0 -o eth0 \
     -s 192.168.1.1/32 -j ACCEPT

iptables -I FORWARD -m conntrack --ctstate RELATED,ESTABLISHED \
     -j ACCEPT

iptables -t nat -I POSTROUTING -o eth0 \
      -s 192.168.1.1/32 -j MASQUERADE
