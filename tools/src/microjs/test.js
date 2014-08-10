

function main(argv, argc)
{
	var i;

	for (i = 0; i < 30; ++i) {
		print('', i);
	}
	return 0;
}


dev = sens[4];
dev.pw5 = 4;
dev.pw5.lookup(32);

dev = sens[4];


function enable_all() 
{
	var i;

	for (i = 0; i < 160; ++i) {
		sens[i].enabled = true;
		mod[i].enabled = true;	
	}
}

function disable_all() 
{
	var i;

	for (i = 0; i < 160; ++i) {
		sens[i].enabled = false;
		mod[i].enabled = false;	
	}
}

function alarm(dev) 
{
	if (dev.enabled) {
		dev.pw5.lookup(2);
		dev.pw2.lookup(2);
	}
}

function alarm_all() 
{
	var i;

	for (i = 0; i < 160; ++i) {
		alarm(sens[i]) 
	}
}

