{
	"sensor": { 
		"id" : 1, 
		"tag" : "2251TM", 
		"desc" : "Multicriteria Photoelectric Smoke Detector", 
		"pw1" : [280, 320],
		"pw2" : [
			{"Not In Remote Test": [280, 320]},
			{"In Remote Test": [560, 640]},
			{"Level 6 / Maintenance Alert Enabled": [840, 960]},
			{"Level 6 / Maintenance Alert Disabled": [1120, 1280]},
		],
		"pw3" : [560, 640],
		"pw4" : [
			{"Normal Photo": [748, 854]},
			{"Low Chamber Trouble": [140, 150]},
			{"Maintenance Urgent/Drift Limit": [280, 320]},
			{"Maintenance Alert": [410, 485]},
			{"Level 1 Alarm - 1% ft": [1308, 1494]},
			{"Level 2 Alarm - Acclimate 1% - 2%": [1680, 1920]},
			{"Level 3 Alarm 3 – 2%/ft.": [2052, 2346,]},
			{"Level 4 Alarm - Acclimate 2% - 4%": [2427, 2774]},
			{"Level 5 Alarm 4%/ft": [2800, 3200]},
			{"Level 6 Alarm – Thermal Alarm (60°C, 135° Heat)": 
				[3250, 3600]},
		],
		"pw5" : [1910, 2240]
	},

	"sensor": { 
		"id" : 2, 
		"tag" : "2251 COPTIR", 
		"desc" : "Multicriteria Smoke Detector", 
		"pw1" : [285, 315],
		"pw2" : [
			{"Remote test disabled": [285, 315]},
			{"Remote test enabled": [570 , 630]},
			{"Level 6 / Maintenance Alert Enabled": [855, 945]},
			{"Level 6 / Maintenance Alert Disabled": [1140, 1260]},
		],
		"pw3" : [570, 630],
		"pw4" : [
			{"Normal, clean": [760, 840]},
			{"CO trouble (sensor life has expired)": [0, 30]},
			{"Low Chamber/Thermistor/CO/IR/Freeze Warning": [140, 160]},
			{"Maintenance Urgent / Drift Limit": [285, 315]},
			{"Maintenance Alert: 80% drift limit": [425, 475]},
			{"CO trouble (6 months of sensor life remaining)": [570, 630]},
			{"IR light saturation condition": [665, 735]},
			{"99% drift (80% drift limit disabled)": [1140, 1260]},
			{"Alarm 1 1%/ft no delays": [1330, 1470]},
			{"Alarm 2 2%/ft no delays": [1710, 1890]},
			{"Alarm 3 3%/ft no delays": [2090, 2310]},
			{"Alarm 4 3%/ft with 10 minutes delay": [2470, 2730]},
			{"Alarm 5 4%/ft with 10 minutes delay": [2850, 3150]},
			{"Alarm 6 Heat": [3230, 3570]},
			{"Remote Test / Magnet Test": [2850, 3150]},
		],
		"pw5" : [1995, 2205]
	},

	"sensor": { 
		"id" : 3, 
		"tag" : "1251", 
		"desc" : "Ionization Smoke Detector", 
		"pw1" : [270, 330],
		"pw2" : [
			{"Remote test disabled": [270, 330]},
			{"Remote test enabled": [540 , 660]},
		],
		"pw3" : [540, 660],
		"pw4" : [
			{"Normal, clean": [760, 840]},
			{"CO trouble (sensor life has expired)": [0, 30]},
			{"Low Chamber/Thermistor/CO/IR/Freeze Warning": [140, 160]},
			{"Maintenance Urgent / Drift Limit": [285, 315]},
			{"Maintenance Alert: 80% drift limit": [425, 475]},
			{"CO trouble (6 months of sensor life remaining)": [570, 630]},
			{"IR light saturation condition": [665, 735]},
			{"99% drift (80% drift limit disabled)": [1140, 1260]},
			{"Alarm 1 1%/ft no delays": [1330, 1470]},
			{"Alarm 2 2%/ft no delays": [1710, 1890]},
			{"Alarm 3 3%/ft no delays": [2090, 2310]},
			{"Alarm 4 3%/ft with 10 minutes delay": [2470, 2730]},
			{"Alarm 5 4%/ft with 10 minutes delay": [2850, 3150]},
			{"Alarm 6 Heat": [3230, 3570]},
			{"Remote Test / Magnet Test": [2850, 3150]},
		],
		"pw5" : [540, 660]
	},

	"module": { 
		"id": 4, 
		"tag": "M500S", 
		"desc": "Supervised Control Module", 
		"pw1": [240, 360], 
		"pw2": 
			[
			{"Reset(Normal)": [240, 360]},
			{"Set": [480, 720]}
		],
		"pw3": 
			[
			{"Open": [480, 720]},
			{"Normal": [240, 360]},
			{"Short": [720, 1080]}
		],
		"pw4": 
			[
			{"Open": [0, 243]},
			{"Normal": [670, 1430]},
			{"Short": [1619, 4000]}
		],
		"pw5": {
			"inf": "Type ID", 
			"pw": [480, 720],
		},
		"ic1": {
			"inf": "Relay pulse"
		},
		"ic2": {
			"inf": "Relay select"
		},
		"ic3": {
			"inf": "LED control"
		}
	},

	"module": { 
		"id": 5, 
		"tag": "M501M", 
		"desc": "Freestanding Minimod", 
		"pw1": {
			"inf": "Reference Pulsewidth",
			"pw": [240, 360], 
		},
		"pw2": {
			"inf": "Class A Fix Status", 
			"pw": [ {"Normal": [240, 360]} ],
		},
		"pw3": {
			"inf": "Supervisory Latch", 
			"pw": [
				{"Open": [480, 720]},
				{"Normal": [240, 360]},
				{"Short,Alarm": [720, 1080]} ]
		},
		"pw4": {
			"inf": "Supervisory (analog)", 
			"pw": [
				{"Open": [0, 220]},
				{"Normal": [756, 1364]},
				{"Short": [1613, 4000]} ],
		},
		"pw5": {
			"inf": "Type ID", 
			"pw": [240, 360],
		},
		"ic1": {
			"inf": "reset PW3 latch"
		},
		"ic2": {
			"inf": "force PW5 = 0"
		},
		"ic3": {
			"inf": "Unused"
		}
	},

	"module": { 
		"id": 6, 
		"tag": "M500M", 
		"desc": "Monitor Module", 
		"pw1": {
			"inf": "Reference Pulsewidth",
			"pw": [240, 360], 
		},
		"pw2": {
			"inf": "Class A Fix Status", 
			"pw": [ 
				{"Normal": [240, 360]},
				{"Fixed": [480, 720]} ]
		},
		"pw3": {
			"inf": "Supervisory Latch", 
			"pw": [
				{"Open": [480, 720]},
				{"Normal": [240, 360]},
				{"Short,Alarm": [720, 1080]} ]
		},
		"pw4": {
			"inf": "Supervisory (analog)", 
			"pw": [
				{"Open": [0, 243]},
				{"Normal": [670, 1430]},
				{"Short": [1619, 4000]} ],
		},
		"pw5": {
			"inf": "Type ID", 
			"pw": [240, 360],
		},
		"ic1": {
			"inf": "reset PW3 latch"
		},
		"ic2": {
			"inf": "force PW5 = 0"
		},
		"ic3": {
			"inf": "Unused"
		}
	},

	"module": { 
		"id": 7, 
		"tag": "M500R", 
		"desc": "Relay Control Module", 
		"pw1": {
			"inf": "Reference Pulsewidth",
			"pw": [240, 360], 
		},
		"pw2": {
			"inf": "Relay Status", 
			"pw": [ 
				{"Reset(Normal)": [240, 360]},
				{"Set": [480, 720]} ]
		},
		"pw3": {
			"inf": "Supervisory Latch", 
			"pw": [
				{"Open": [480, 720]} ]
		},
		"pw4": {
			"inf": "Supervisory (analog)", 
			"pw": [
				{"Open": [0, 343]} ],
		},
		"pw5": {
			"inf": "Type ID", 
			"pw": [480, 720],
		},
		"ic1": {
			"inf": "Relay Pulse"
		},
		"ic2": {
			"inf": "Relay select"
		},
		"ic3": {
			"inf": "LED control"
		}
	}
}

