var stdout_flush_tmo = [500, 500, 500, 500, 500];

function simlib_stdout_flush(port, pre) {
	$.ajax({
		type: 'GET',
		url: 'get_stdout.cgi',
		data: { 
			port: port,
		},
		dataType: 'text',
		timeout: 100,
		context: $('body'),
		success: function(data) {
			if (data) {
				pre.append(data);
				pre.scrollTop(pre.prop("scrollHeight"));
				stdout_flush_tmo[port] = 500;
			}
			setTimeout(simlib_stdout_flush, stdout_flush_tmo[port], port, pre);
		},
		error: function(xhr, type) {
			setTimeout(simlib_stdout_flush, 5000, port, pre);
		},
		complete: function(xhr, status) {
		}
	});
};

function simlib_status_get(port, tr) {	
	$.ajax({
		type: 'GET',
		url: 'get_status.cgi',
		data: { 
			port: port
		},
		dataType: 'json',			
		timeout: 250,
		context: $('body'),
		success: function(data) {
			tr.cells[1].innerHTML = data.state;
			if (data.hasOwnProperty('kernel')) {
				tr.cells[2].innerHTML = 'ThinkOS-' + data.kernel.version.major + '.' + 
					data.kernel.version.minor + '.' +
					data.kernel.version.build;
				tr.cells[3].innerHTML = data.kernel.ticks;
			} else {
				tr.cells[2].innerHTML = '?.?.?';
				tr.cells[3].innerHTML = '?';
			}
			if (data.hasOwnProperty('app')) {
				tr.cells[4].innerHTML = 'DevSim-' + data.app.version.major + '.' + 
					data.app.version.minor + '.' +
					data.app.version.build;
				tr.cells[5].innerHTML = data.app.uptime;						
			} else {
				tr.cells[4].innerHTML = '?.?.?';
				tr.cells[5].innerHTML = '?';
			}
		},
		error: function(xhr, type) { 
			stdout_flush_tmo[port] = 5000;
		}
	});
}

function simlib_js_post(port, script) {		
	$.ajax({
 		type: 'POST',
		url: 'rpc_js.cgi?port=' + port,
 		data: script,
		dataType: 'text',
		timeout: 250,
		context: $('body'),
		contentType: 'json',
		success: function(data) {
   		},
		error: function(xhr, type){
			stdout_flush_tmo[port] = 5000;
		},
		complete: function() {
		}
	});
};

function alert_show(__box, __msg) {		
	var html = '<img style="float:none;vertical-align:middle"' +
		'src="/img/alert.png" alt="Alert! "/>&nbsp;' + __msg;
	__box.innerHTML = html;
}

function alert_hide(__box) {
	__box.innerHTML = "";
}

function simlib_edit_file_read(port, fname, editor) {
	editor.text('');	
	$('.button').prop('disabled', true);
	$.ajax({
		type: 'GET',
		url: 'file_read.cgi',
		data: { 
			fname: fname,
			port: port,
		},
		dataType: 'text',
		timeout: 5000,
/*		async: false, */
		context: $('body'),
		success: function(data) {
			editor.text(data);
		},
		error: function(xhr, type) {
			alert('Ajax error!')
		},
		complete: function(xhr, status) {
			$('.button').prop('disabled', false);
		}
	});
}

function simlib_edit_file_upload(port, editor, __form, __box) {
	editor.text('');
	
	if (!(__box === undefined)) {
		alert_show(__box, "Uploading file, please wait...");
	}
	
	$('.button').prop('disabled', true);
	
	$.ajax({
		url: __form.action + '?port=' + port,
		type: 'POST',
		data: new FormData(__form),
		processData: false,
		contentType: false,
		/* async: false, */
		success: function(data) {				
			editor.text(data);
		},
		error: function(xhr, type) {
			alert('Ajax error!')
		},
		complete: function(xhr, status) {
			if (!(__box === undefined)) {
				alert_hide(__box);
			}
			$('.button').prop('disabled', false);
		}
	});
	
	event.preventDefault();
}

function simlib_edit_file_write(port, fname, editor, __box) {	
	var text = editor.val();
	
	if (!(__box === undefined)) {
		alert_show(__box, "Uploading file, please wait...");
	}
	
	$('.button').prop('disabled', true);	

	$.ajax({
 		type: 'POST',
		url: 'file_write.cgi?port=' + port + '&fname=' + fname,
 		data: text,
		dataType: 'text',
		timeout: 5000,
/*		async: false, */
		context: $('body'),
		contentType: 'text/plain',
		success: function(data) {
   		},
		error: function(xhr, type){
			stdout_flush_tmo[port] = 5000;
		},
		complete: function() {
			if (!(__box === undefined)) {
				alert_hide(__box);
			}
			$('.button').prop('disabled', false);			
		}
	});
};
