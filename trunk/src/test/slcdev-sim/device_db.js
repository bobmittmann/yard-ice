{
	"sensor": { 
		"id" : 1, 
		"model" : "2251TM", 
		"desc" : "Multicriteria Photoelectric Smoke Detector", 
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
		"id" : 2, 
		"model" : "2251 COPTIR", 
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
		"id" : 3, 
		"model" : "1251", 
		"desc" : "Ionization Smoke Detector", 
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
		"id" : 4, 
		"model" : "5251P", 
		"desc" : "Heat Detector", 
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
		"id" : 5, 
		"model" : "5251T", 
		"desc" : "Heat Detector", 
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
		"id": 6, 
		"model": "M500S", 
		"desc": "Supervised Control Module", 
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
		"pw5": ["Type ID", 480, 720],
		"ic1": [ "Relay pulse" ],
		"ic2": [ "Relay select" ],
		"ic3": [ "LED control" ]
	},

	"module": { 
		"id": 7, 
		"model": "M501M", 
		"desc": "Freestanding Minimod", 
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
		"pw5": [ "Type ID", 240, 360],
		"ic1": [ "reset PW3 latch" ],
		"ic2": [ "force PW5 = 0" ],
		"ic3": [ "Unused" ]
	},

	"module": { 
		"id": 8, 
		"model": "M500M", 
		"ap": false,
		"desc": "Monitor Module", 
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
		"pw5": ["Type ID", 240, 360],
		"ic1": ["reset PW3 latch", 1],
		"ic2": ["force PW5 = 0", 1 ],
		"ic3": ["Unused", 0]
	},

	"module": { 
		"id": 9, 
		"model": "M500R", 
		"desc": "Relay Control Module", 
		"pw1": [240, 360],
		"pw2": [
			["Reset(Normal)", 240, 360],
			["Set", 480, 720]
		],
		"pw3": ["Open", 480, 720],
		"pw4": ["Open", 0, 343],
		"pw5": [480, 720],
		"ic1": ["Relay Pulse"],
		"ic2": ["Relay select"],
		"ic3": ["LED control"],
	},

	"module": { 
		"id": 10, 
		"model": "M500FP", 
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
		"pw5": [1750, 2025],
		"ic1": ["Relay Pulse"],
		"ic2": ["Relay select"],
		"ic3": ["LED control"]
	}

}

