export default {
  async fetch(request, env) {
    const { pathname } = new URL(request.url);
    if (pathname === "/api/fetch-daemon-job") {
        const data = await env.DB.prepare(
          "SELECT JOB_ID, command FROM runnable WHERE status = ?"
        ).bind('CREATED').all();
        await env.DB.prepare(
            "UPDATE runnable SET status='SENT TO DAEMON' WHERE status = ?"
        ).bind('CREATED').all()
        const response = {
            statusCode: 200,
            body: data.results,
        };
        return Response.json(response);
    } else {
        const response = {
            statusCode: 204,
            body: 'Use /api/fetch-daemon-job',
        };
        return Response.json(response);
    }
  },
};