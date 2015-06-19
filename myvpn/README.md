# 構造
* 2スレッド構造
  * tunを監視→IPパケットが流れてきたら、UDPにカプセル化してVPNサーバに送信
  * VPNソケット監視→UDPが流れてきたら、ペイロード部分をtunに流す
* file descripterはselectで監視
  * チューニングの余地→epoll
* 課題
  * latencyがある（1ms+）
  * 謎のパケットロス？
  
# 使い方
## myvpn.c (firewall通過不可)
myvpn.cはサーバー・クライアントの概念はありません。Peer to Peerな関係です。
通信は一対一で、相互とも相手を指定する必要があります。また、NAT環境下などでは利用できません。
どちらからもUDPが届く必要があります。


ピアA
```
./myvpn -a 172.16.5.129 -p 12345 -t "ifconfig %s 192.168.1.1 mtu 1450  pointopoint 192.168.1.2"
```
ピアB
```
./myvpn -a 172.16.5.1 -p 12345 -t "ifconfig %s 192.168.1.2 mtu 1450 pointopoint 192.168.1.1" 
```

-aが相手のIPアドレス、pが接続先ポートです。申し訳ないがリッスンポート12345はハードコードされていて引数からのインターフェイスはない。
このプログラムを実行するとPeer-to-peerなTUNが作成されます。tunのMTUは1450で、理由は、標準的な1500にしてしまうと、それに対してUDPヘッダをエンカプセレーションで付けてしまって1500以上になり、1つの元のパケットが2つに別れてしまうフラグメントが発生します。
そうなるとオーバーヘッドになるので、tunを最初から1450にしてそういうことを防止しています。

## myvpn\_nat.c (おそらくfirewall/NAT通過可能)
クライアントからサーバへ接続し、サーバはそれで得たソースポートに送る。
クライアントの最初のパケットで、firewallに穴があき、NATに変換テーブルが作られるため、おそらく普通のネットワークなら利用できる。
### サーバ
```
./myvpn_nat -s 12345 -t "ifconfig %s 192.168.1.2 mtu 1450 pointopoint 192.168.1.1"
```
* -s バインドポート（このUDPポートでリッスン）
* -t ifconfig 設定文字列。
  * MTUはLAN内なら1450くらいでOK（詳しく調べてない）
  * フレッツ光ネクスト 1412 （1454からIPヘッダ20とUDPヘッダ8を引いて、Ethernet Frame 14bytes引く）でフラグメント発生なし。調査はtcpdumpで行った
```
tcpdump -vvv -i eth0 host 160.16.95.238 and not port 22 and \("ip[6:2] & 0x2000 == 0x2000" or "ip[6:2] & 0x1fff != 0x0000"\)
```
### クライアント
```
./myvpn_nat -a 160.16.95.238 -p 12345 -t "ifconfig %s 192.168.1.1 mtu 1450  pointopoint 192.168.1.2"
```
* -a サーバのIPアドレス
* -p サーバーのポート

# 備考
## サーバをデフォルトゲートウェイ化
### クライアント側
VPNサーバへのmyvpnパケット以外全部tun0(192.168.1.2)に流します。

```
sudo route add -host [VPNサーバのIPアドレス] gw [いつも使ってるデフォルトゲートウェイ] dev [eth0などデバイス名]
sudo route add default gw 192.168.1.2
```

### サーバー側
サーバのtunに到着したパケットをeth0から出るようにしてNAT（ipマスカレード）するようにします。

```
iptables -I FORWARD -i tun0 -o eth0 \
     -s 192.168.1.1/32 -j ACCEPT

iptables -I FORWARD -m conntrack --ctstate RELATED,ESTABLISHED \
     -j ACCEPT

iptables -t nat -I POSTROUTING -o eth0 \
      -s 192.168.1.1/32 -j MASQUERADE
```
## myvpn\_aes.c (secured by OpenSSL!)
いちパケットごとに同じIV使うとやばそう。
となると、あとのパケットは前のパケットの暗号の状態を利用する実装にしたい。
が、到着順が保証されないUDPだと無理ではないかな。
### 解決策？
 * TCPにする（遅い）

