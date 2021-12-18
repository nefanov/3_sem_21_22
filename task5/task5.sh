gcc task5.c
./a.out im.png im1.png

origin=$(md5 im.png | grep -o '=.*')

copy=$(md5 im1.png | grep -o '=.*')

if [ "$origin" = "$copy" ];then
  echo "files are equal";
else
  echo "files are not equal";
fi