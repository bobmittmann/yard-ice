{
	"sensor": { 
		"model" : "Photo", 
		"desc" : "Photoelectric Smoke Detector", 
		"sim" : "photo", 
		"pw1" : [270, 330],
		"pw2" : [
			["Remote test disabled", 270, 330],
			["Remote test enabled", 540, 660]
		],
		"pw3" : [540, 660],
		"pw4" : [
			["Normal", 618, 1068],
			["Trouble", 120, 660],
			["Smoke Alarm 1", 1260, 1775],
			["Smoke Alarm 2", 1775, 2395],
			["Smoke Alarm 3", 2395, 2800],
			["Thermal Alarm (57°C)", 2800, 3220],
			["Remote Test", 2800, 3200]
		],
		"pw5" : [810, 990]
	},

	"sensor": { 
		"model" : "Ion", 
		"desc" : "Ionization Smoke Detector", 
		"sim" : "ion", 
		"pw1" : [270, 330],
		"pw2" : [
			["Remote test disabled", 270, 330],
			["Remote test enabled", 540, 660]
		],
		"pw3" : [540, 660],
		"pw4" : [
			["Normal", 870, 930],
			["Low Chamber", 120, 180],
			["High Chamber", 260, 330],
			["Smoke Alarm 1", 1260, 1775],
			["Smoke Alarm 2", 1775, 2395],
			["Smoke Alarm 3", 2395, 2800],
			["Remote Test", 2130, 2880]
		],
		"pw5" : [570, 630]
	},

	"sensor": { 
		"model" : "Heat", 
		"desc" : "Heat Detector", 
		"sim" : "heat", 
		"pw1" : [270, 330],
		"pw2" : [
			["Remote test disabled", 270, 330],
			["Remote test enabled", 540, 660]
		],
		"pw3" : [540, 660],
		"pw4" : [
			["Normal, 25°C", 1070, 1400],
			["Normal, 57.2°C", 1895, 2110],
			["Heat Alarm 57.2°C", 2110],
			["Remote Test", 3270, 3330]
		],
		"pw5" : [270, 330],
		"cmd": [
			{
				"tag": "LED on",
				"seq": ["0xx"],
				"js": [ "dev.led_on = true;" ]
			},
			{
				"tag": "LED off",
				"seq": ["1x0"],
				"js": [ "dev.led_on = false;" ]
			},
			{
				"tag": "LED blink",
				"seq": ["1x1"],
				"js": [ "dev.led_blink = true;" ]
			},
			{
				"tag": "Enable Type ID",
				"seq": ["x1x"],
				"js": [ "dev.pw5_en = true;" ]
			},
			{
				"tag": "Remote Test on",
				"seq": ["0x0", "0x0"],
				"js": [ "dev.test = true;" ]
			},
			{
				"tag": "Remote Test off",
				"seq": ["1x1", "1x1"],
				"js": [ "dev.test = false;" ]
			}
		]
	},

	"sensor": { 
		"model" : "2251TM", 
		"desc" : "Multicriteria Photoelectric Smoke Detector", 
		"sim" : "acclimate", 
		"pw1" : [280, 320],
		"pw2" : [
			["Not In Remote Test", 280, 320],
			["In Remote Test", 560, 640],
			["Level 6 / Maintenance Alert Enabled", 840, 960],
			["Level 6 / Maintenance Alert Disabled", 1120, 1280],
		],
		"pw3" : [560, 640],
		"pw4" : [
			["Normal Photo", 748, 854],
			["Low Chamber Trouble", 140, 150],
			["Maintenance Urgent/Drift Limit", 280, 320],
			["Maintenance Alert", 410, 485],
			["Level 1 Alarm - 1% ft", 1308, 1494],
			["Level 2 Alarm - Acclimate 1% - 2%", 1680, 1920],
			["Level 3 Alarm 3% 2%/ft.", 2052, 2346,],
			["Level 4 Alarm - Acclimate 2% - 4%", 2427, 2774],
			["Level 5 Alarm 4%/ft", 2800, 3200],
			["Level 6 Alarm - Thermal Alarm (60°C, 135° Heat)", 
			3250, 3600],
		],
		"pw5" : [1910, 2240]
	},

	"sensor": { 
		"model" : "2251 COPTIR", 
		"sim" : "coptir", 
		"desc" : "Multicriteria Smoke Detector", 
		"pw1" : ["Reference", 285, 315],
		"pw2" : [
			["Remote test disabled", 285, 315],
			["Remote test enabled", 570 , 630],
			["Level 6 / Maintenance Alert Enabled", 855, 945],
			["Level 6 / Maintenance Alert Disabled", 1140, 1260],
		],
		"pw3" : [570, 630],
		"pw4" : [
			["Normal, clean", 760, 840],
			["CO trouble (sensor life has expired)", 0, 30],
			["Low Chamber/Thermistor/CO/IR/Freeze Warning", 140, 160],
			["Maintenance Urgent / Drift Limit", 285, 315],
			["Maintenance Alert: 80% drift limit", 425, 475],
			["CO trouble (6 months of sensor life remaining)", 570, 630],
			["IR light saturation condition", 665, 735],
			["99% drift (80% drift limit disabled)", 1140, 1260],
			["Alarm 1 1%/ft no delays", 1330, 1470],
			["Alarm 2 2%/ft no delays", 1710, 1890],
			["Alarm 3 3%/ft no delays", 2090, 2310],
			["Alarm 4 3%/ft with 10 minutes delay", 2470, 2730],
			["Alarm 5 4%/ft with 10 minutes delay", 2850, 3150],
			["Alarm 6 Heat", 3230, 3570],
			["Remote Test / Magnet Test", 2850, 3150]
		],
		"pw5" : [1995, 2205]
	},

	"sensor": { 
		"model" : "1251", 
		"desc" : "Ionization Smoke Detector", 
		"sim" : "ion", 
		"pw1" : [270, 330],
		"pw2" : [
			["Remote test disabled", 270, 330],
			["Remote test enabled", 540 , 660],
		],
		"pw3" : [540, 660],
		"pw4" : [
			["Normal, clean", 760, 840],
			["CO trouble (sensor life has expired)", 0, 30],
			["Low Chamber/Thermistor/CO/IR/Freeze Warning", 140, 160],
			["Maintenance Urgent / Drift Limit", 285, 315],
			["Maintenance Alert: 80% drift limit", 425, 475],
			["CO trouble (6 months of sensor life remaining)", 570, 630],
			["IR light saturation condition", 665, 735],
			["99% drift (80% drift limit disabled)", 1140, 1260],
			["Alarm 1 1%/ft no delays", 1330, 1470],
			["Alarm 2 2%/ft no delays", 1710, 1890],
			["Alarm 3 3%/ft no delays", 2090, 2310],
			["Alarm 4 3%/ft with 10 minutes delay", 2470, 2730],
			["Alarm 5 4%/ft with 10 minutes delay", 2850, 3150],
			["Alarm 6 Heat", 3230, 3570],
			["Remote Test / Magnet Test", 2850, 3150]
		],
		"pw5" : [540, 660]
	},

	"sensor": { 
		"model" : "5251P", 
		"desc" : "Heat Detector", 
		"sim" : "heat", 
		"pw1" : [270, 330],
		"pw2" : [
			["Remote test disabled", 270, 330],
			["Remote test enabled", 540 , 660],
		],
		"pw3" : [540, 660],
		"pw4" : [
			["Normal, 25°C", 1070, 1400],
			["Normal, 57.2°C", 1895, 2110],
			["Remote Test", 3250, 3330]
		],
		"pw5" : [270, 330]
	},

	"sensor": { 
		"model" : "5251T", 
		"desc" : "Heat Detector", 
		"sim" : "heat", 
		"pw1" : [270, 330],
		"pw2" : [
			["Remote test disabled", 270, 330],
			["Remote test enabled", 540 , 660],
		],
		"pw3" : [540, 660],
		"pw4" : [
			["Normal, 25°C", 1070, 1400],
			["Normal, 57.2°C", 1895, 2110],
			["Remote Test", 3250, 3330]
		],
		"pw5" : [270, 330]
	},

	"module": { 
		"model": "M500S", 
		"desc": "Supervised Control Module", 
		"sim" : "control", 
		"pw1": [240, 360], 
		"pw2": [ 
			["Reset(Normal)", 240, 360],
			["Set", 480, 720] 
		],
		"pw3": [
			["Open", 480, 720],
			["Normal", 240, 360],
			["Short", 720, 1080]
		],
		"pw4": [
			["Open", 0, 243],
			["Normal", 670, 1430],
			["Short", 1619, 4000]
		],
		"pw5": ["Type ID", 480, 720]
	},

	"module": { 
		"model": "M501M", 
		"desc": "Freestanding Minimod", 
		"sim" : "mini", 
		"pw1": [240, 360], 
		"pw2": ["Normal", 240, 360 ],
		"pw3": [
			["Open", 480, 720],
			["Normal", 240, 360],
			["Short,Alarm", 720, 1080]
		],
		"pw4": [
			["Open", 0, 220],
			["Normal", 756, 1364],
			["Short", 1613, 4000] 
		],
		"pw5": [ "Type ID", 240, 360]
	},

	"module": { 
		"model": "M500M", 
		"ap": false,
		"desc": "Monitor Module", 
		"sim" : "monitor", 
		"pw1": ["Reference Pulsewidth", 240, 360], 
		"pw2": [
			["Normal", 240, 360],
			["Fixed", 480, 720]
		],
		"pw3": [
			["Open", 480, 720],
			["Normal", 240, 360],
			["Short,Alarm", 720, 1080]
		],
		"pw4": [
			["Open", 0, 243],
			["Normal", 670, 1430],
			["Short", 1619, 4000]
		],
		"pw5": ["Type ID", 240, 360]
	},

	"module": { 
		"model": "M500R", 
		"sim" : "relay", 
		"desc": "Relay Control Module", 
		"pw1": [240, 360],
		"pw2": [
			["Reset(Normal)", 240, 360],
			["Set", 480, 720]
		],
		"pw3": ["Open", 480, 720],
		"pw4": ["Open", 0, 343],
		"pw5": [480, 720]
	},

	"module": { 
		"model": "M500FP", 
		"sim" : "phone", 
		"desc": "AOM-Tel Firephone Module", 
		"pw1": [240, 360],
		"pw2": [
			["Reset(Normal)", 240, 360],
			["Set", 560, 630]
		],
		"pw3": [
			["Trouble (Short or Open)", 560, 630],
			["Normal", 240, 360],
			["Off-Hook", 860, 940]
		],
		"pw4": [0, 300],
		"pw5": [1750, 2025]
	},

	"sensor": { 
		"model": "BEAM200", 
		"sim" : "beam", 
		"desc": "200 Series Beam Detector",
		"pw1": [300],
		"pw2": [
			["Remote test disabled", 300],
			["Remote test enabled", 600],
			["Remote test with features", 900],
		],
		"pw3" : [600],
		"pw4": [
			["(Normal)	Analog Beam", 900],
			["(Trouble, Beam Blockage)  95 to 100 % obscuration", 30],
			["(Trouble, Beam signal over range)", 150],
			["(Alignment mode)", 300],
			["(Initialization and Obscuration test modes)", 750],
			[">10% of total drift compensation range used", 945],
			[">20% of total drift compensation range used", 990],
			[">30% of total drift compensation range used", 1035],
			[">40% of total drift compensation range used", 1080],
			[">50% of total drift compensation range used", 1125],
			[">60% of total drift compensation range used", 1170],
			[">70% of total drift compensation range used", 1215],
			[">80% of total drift compensation range used", 1260],
			[">90% of total drift compensation range used", 1305],
			[">100% of total drift compensation range used", 150],
			["Remote Test and Alarm 25% sensitivity setting", 2700], 
			["Remote Test and Alarm 30% sensitivity setting", 2790],
			["Remote Test and Alarm 40% sensitivity setting", 2880],
			["Remote Test and Alarm 50% sensitivity setting", 2970],
			["Remote Test and Alarm Acclimate level 1 (30% to 50%)", 3060],
			["Remote Test and Alarm Acclimate level 2 (40% to 50%)", 3150],
		],
		"pw5": [900],
		"cmd": [
			{
				"tag": "Reset",
				"seq": ["0x0", "1x0", "0x1", "0x0", "1x0"],
				"js": [
					"dev.pw2 = mdl.pw2[0];",
					"dev.pw4 = mdl.pw4[0];"
					]
			},
			{
				"tag": "Obscuration Test",
				"seq": ["0x0", "0x1", "0x1", "0x0", "1x0"],
				"js": [
					"dev.pw2 = mdl.pw2[1];",
					"dev.pw4 = mdl.pw4[4];"
					]
			}
		]
	}
}

