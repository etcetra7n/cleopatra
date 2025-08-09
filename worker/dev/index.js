export default {
  async fetch(request, env) {
    const authkey = request.headers.get("Authorization");
    if (authkey !== env.DAEMON_KEY) {
      return new Response("Invalid key", {status: 401});
    }
    const { pathname } = new URL(request.url);
    if (pathname === "/api/fetch") {
        const MACHINE_ID  = request.headers.get("MID");
        const data = await env.DB.prepare(
          "SELECT JOB_ID, command, file FROM runnable WHERE ((status='CREATED' OR status IS NULL) AND (TARGET_MACHINE=? OR TARGET_MACHINE='ANY'))"
        ).bind(MACHINE_ID).all();
        
        if (data.results.length !== 0){
            await env.DB.prepare(
                "UPDATE runnable SET status='SENT TO DAEMON' WHERE ((status='CREATED' OR status IS NULL) AND (TARGET_MACHINE=? OR TARGET_MACHINE='ANY'))"
            ).bind(MACHINE_ID).all();
        }
        const response = {
            statusCode: 200,
            body: data.results,
        };
        return Response.json(response);
    } else if (pathname === "/api/send") {
        const result = await request.json()
        const newStatus = (result.exitCode===0)?"SUCCESS":"FAILURE";
        const data = await env.DB.prepare(
          "UPDATE runnable SET status=?, output=?, exit_code=? WHERE JOB_ID=?"
        ).bind(newStatus, result.output, result.exitCode, result.JID).all();
        const response = {
            statusCode: 200,
        };
        return Response.json(response);
    } 
    else {
        const response = {
            statusCode: 204,
            body: 'Invalid api',
        };
        return Response.json(response);
    }
  },
};