var x, y, z;

sensor[3].alarm = 2;
sensor[3].trouble = 1;
x = sensor[3].state;
print(x);

module[13].alarm = 3;
module[13].trouble = 2;
y = module[13].state;

print(y);

mod_alarm(13, 3);
mod_trouble(13, 2);
z = mod_state(13);

print(z);

print(x, y, z);

//printf("x=%d, y=%d, %d, %d\n", x, y, 11, 12);

