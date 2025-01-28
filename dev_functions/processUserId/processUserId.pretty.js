const admin = require('firebase-admin');
const serviceAccount = require('../xthreadspro-firebase-adminsdk-ty40a-3df7f647f5.json');

if (!admin.apps.length) {
  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount)
  });
}
const db = admin.firestore();

function enterUserToDatabase(userData){
  try{
      let plan = "";
      const userRef = db.collection('users').doc(userData.uid);
      const docSnapshot = userRef.get();
      if (docSnapshot.exists) {
          userRef.update({
            "last_login_time": admin.firestore.FieldValue.serverTimestamp(),
            "no_of_logins": admin.firestore.FieldValue.increment(1),
          });
          plan = userRef.get("plan");
      } else {
          plan = "basic";
          userRef.set({
            "name": userData.name,
            "last_login_time": admin.firestore.FieldValue.serverTimestamp(),
            "email": userData.email,
            "uid": userData.uid,
            "picture": userData.picture,
            "plan": "basic",
            "last_plan_purchase_time": admin.firestore.FieldValue.serverTimestamp(),
            "no_of_logins": 1,
            "account_creation_time": admin.firestore.FieldValue.serverTimestamp(),
          });
      }
      return plan;
  } catch (error) {
      throw error;
  }
}

exports.handler = async (event, context) => {
  const commonHeaders = {
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'POST, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type'
  };
  if (event.httpMethod == 'OPTIONS') {
    return {
      statusCode: 200,
      headers: commonHeaders,
      body: JSON.stringify({ message: "success" })
    };
  }
  const userIdToken = JSON.parse(event.body).userIdToken;
  try {
    // Verify the ID token
    const decodedToken = await admin.auth().verifyIdToken(userIdToken);
    const plan = enterUserToDatabase(await decodedToken);
    return await {
      statusCode: 200,
      headers: commonHeaders,
      body: JSON.stringify({ 
        "email": decodedToken.email,
        "uid": decodedToken.uid,
        "name": decodedToken.name,
        "picture": decodedToken.picture,
        "plan": plan,
      }),
    };
  } catch (error) {
    console.error('Error verifying ID token:', error);
    return {
      statusCode: 401,
      headers: commonHeaders,
      body: JSON.stringify({ message: "Invalid" }),
    };
  }
};
