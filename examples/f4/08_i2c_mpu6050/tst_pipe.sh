#!/bin/bash
P='/dev/ttyACM0'

stty -F $P speed 115200
frst='y'
tp=0
n=0
> o1.txt

while : ; do
  echo '*** SEND cmd ****'
  echo 'D' > $P
  fo='n'

  while read -t0.1 -a ma < $P ; do
    echo "__ prep: \"${ma[*]}\" "
    c0="${ma[0]:0:1}"
    if [[ "$c0" == '@' ]] ; then
      fo='y'
      break;
    fi
  done

  if [[ "$fo" != 'y' ]] ; then
    echo ".... RESEND .... ma[]= \"${ma[*]}\""
    echo ".... RESEND .... ma[]= \"${ma[*]}\"" > /dev/stderr
    continue
  fi

  read -t0.01 tmp < $P # eat "rc="
  # read -a ma <<< "$a"

  if [[ $frst == 'y' ]] ; then
    t0="${ma[1]}"
  fi

  frst='n'
  tc="${ma[1]}"
  if [[ ! "$tc" =~ ^[01234456789]+ ]] ; then
    echo ".... RESEND2 .... ma[]= \"${ma[*]}\""
    echo ".... RESEND2 .... ma[]= \"${ma[*]}\"" > /dev/stderr
    continue
  fi

  t=$(( ma[1] - t0 ))
  dt=$(( t - tp ))
  tp="$t"
  echo "<< n: $n 0: ${ma[0]} t: $t dt: $dt 2: ${ma[2]} ${ma[3]} ${ma[4]} >>";
  echo "$t $dt ${ma[2]} ${ma[3]} ${ma[4]} ${ma[5]} ${ma[6]} ${ma[7]} ${ma[8]}" >> o1.txt
  echo
  #echo $n > /dev/stderr
  ((++n))
  # usleep 100
done

