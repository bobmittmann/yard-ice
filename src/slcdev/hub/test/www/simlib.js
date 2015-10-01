var stdout_flush_tmo[5];

function get_stdout(pre, port) {
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
				stdout_flush_tmo = 500;
			}
			setTimeout(get_stdout, stdout_flush_tmo[port], pre, port);
		},
		error: function(xhr, type){
			setTimeout(get_stdout, 5000, pre, port);
		},
		complete: function(xhr, status){
		}
	});
};
