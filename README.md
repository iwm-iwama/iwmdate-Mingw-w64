### 実行に必要なファイル
- iwmdateadd.exe
- iwmdatediff.exe

---

### このプログラムについて
#### 1.日付計算: iwmdateadd.exe
  - (例) 現在時 "." の 1年2月3日後 を計算
    - iwmdateadd "." -y=1 -m=2 -d=3 -f="%g%y-%m-%d(%a) %h:%n:%s"

#### 2.日付差分: iwmdatediff.exe
  - (例) 現在時 "." から "2000/1/1" の差分
    - iwmdatediff "." "2000/1/1" -f="%g%y-%m-%d %h:%n:%s"
