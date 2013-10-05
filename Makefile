all:
	gcc first-friday.c -fno-stack-protector -z execstack -o 100-first-friday
	gcc catsforgold.c -fno-stack-protector -z execstack -o 200-catsforgold
	gcc mahbukkit.c -fno-stack-protector -z execstack -o 300-mahbukkit

clean:
	rm -f 100-first-friday
	rm -f 200-catsforgold
	rm -f 300-mahbukkit
