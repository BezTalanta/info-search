g++ main.cpp
if [ $? -gt 0 ];
then
	exit
else
	./a.out
fi
