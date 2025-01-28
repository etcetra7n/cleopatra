const { GoogleGenerativeAI } = require("@google/generative-ai");
const admin = require('firebase-admin');

const serviceAccount = require('../xthreadspro-firebase-adminsdk-ty40a-3df7f647f5.json');

if (!admin.apps.length) {
  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount)
  });
}
const db = admin.firestore();

async function logTopic(topic) {
    try{
        await db.collection('topics').doc(topic).set({
          "topic": topic,
          "timestamp": admin.firestore.FieldValue.serverTimestamp(),
        });
        return;
    } catch (error) {
        throw error;
    }
}

async function generateThread(topic, num, creativity, type){
  const genAI = new GoogleGenerativeAI(process.env.GEMINI_API_KEY);
  const model = genAI.getGenerativeModel({ model: "gemini-1.5-flash"});
  if (creativity=='default'){
    var creativity_indicator_text = "";
  } else if(creativity=='low'){
    var creativity_indicator_text = "Keep it simple. ";
  } else if(creativity=='high'){
    var creativity_indicator_text = "Make it creative. ";
  }
  if(type=='educative'){
    var prompt = `Generate a twitter thread for the topic "${topic}". This is for educational purpose. Include facts and other interesting things to read. Keep it straightforward and don't use any emojis. It should sound like a ${type} article. ${creativity_indicator_text}The thread should have ${num} tweets including catchy starting tweet. Give result in a JSON format`;
  } else {
    var prompt = `Generate a twitter thread for the topic "${topic}". Include facts and other interesting things to read. Keep it straightforward and don't use any emojis. It should sound like a ${type} article. ${creativity_indicator_text}The thread should have ${num} tweets including catchy starting tweet. Give result in a JSON format`;
  }

  const result = await model.generateContent(prompt);
  const response = await result.response;
  const text = response.text();
  return text;
}

exports.handler = async (event) => {
  const commonHeaders = {
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'POST, OPTIONS, GET',
    'Access-Control-Allow-Headers': 'Content-Type'
  };
  if (event.httpMethod == 'OPTIONS') {
    return {
      statusCode: 200,
      headers: commonHeaders,
      body: JSON.stringify({ message: "success" })
    };
  }
  const params = JSON.parse(event.body);
  try {
    await logTopic(params.topic);
    const response = await generateThread(params.topic, params.num, params.creativity, params.type);
    const result = JSON.parse(response.substring(8, response.length-4));
      return {
        statusCode: 200,
        headers: {
          'Access-Control-Allow-Origin': '*',
          'Access-Control-Allow-Headers': 'Content-Type',
        },
        body: JSON.stringify(result)
      };
  } catch (error) {
    console.log(error);
    return {
      statusCode: 500,
      headers: {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Headers': 'Content-Type',
      },
      body: JSON.stringify({ "error": "Internal server error" })
    };
  }
};