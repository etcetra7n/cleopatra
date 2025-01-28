const admin = require('firebase-admin');
const serviceAccount = require('../xthreadspro-firebase-adminsdk-ty40a-3df7f647f5.json');

if (!admin.apps.length) {
  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount)
  });
}
const db = admin.firestore();

/*PAYPAL_CLIENT_ID;
PAYPAL_CLIENT_SECRET;*/
/*
const price = {
 'versatile': 8,
 'effective': 11,
};*/

/*const generateAccessToken = async () => {
  try {
    const auth = Buffer.from(
      PAYPAL_CLIENT_ID + ":" + PAYPAL_CLIENT_SECRET
    ).toString("base64");
    const response = await fetch(`https://api-m.sandbox.paypal.com/v1/oauth2/token`, {
      method: "POST",
      body: "grant_type=client_credentials",
      headers: {
        Authorization: `Basic ${auth}`,
      },
    });

    const data = await response.json();
    return data.access_token;
  } catch (error) {
    console.error("Failed to generate Access Token:", error);
  }
};*/

async function updatePlan(uid, plan){
  try{
      const userRef = db.collection('users').doc(uid);

      let planExpiryDate = new Date();
      planExpiryDate.setMonth(planExpiryDate.getMonth() + 1);

      await userRef.update({
        "last_plan_purchase_time": admin.firestore.FieldValue.serverTimestamp(),
        "plan_expiry_date": planExpiryDate,
        "plan": plan,
      });
  } catch (error) {
      throw error;
  }
}
async function logPayment(uid, plan, orderDetails) {
    try{
        await db.collection('payment_log').add({
          "uid": uid,
          "plan": plan,
          "status": orderDetails.status,
          "statusText": orderDetails.statusText,
          "timestamp": admin.firestore.FieldValue.serverTimestamp(),
        });
        return;
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
  const data = JSON.parse(event.body);
  /*const accessToken = await generateAccessToken();*/
  /*const payload = {
    intent: "CAPTURE",
    purchase_units: [
      {
        amount: {
          currency_code: "USD",
          value: price[data.plan],
        },
      },
    ],
  };*/
  try {
    const auth = Buffer.from(
      PAYPAL_CLIENT_ID + ":" + PAYPAL_CLIENT_SECRET
    ).toString("base64");
    const orderDetails = await fetch(`https://api-m.paypal.com/v2/checkout/orders/${data.orderId}/capture`, {
        method: 'POST',
        headers: {
            "Content-Type": "application/json",
            "Authorization": `Basic ${auth}`,
          },
    });
    await logPayment(data.uid, data.plan, await orderDetails);
    console.log(orderDetails);
    if (orderDetails.ok){
      await updatePlan(data.uid, data.plan);
    }
    return {
      statusCode: await orderDetails.status,
      headers: commonHeaders,
      body: JSON.stringify({orderDetails}),
    };
  } catch (error) {
    console.error('Error verifying ID token:', error);
    return {
      statusCode: 500,
      headers: commonHeaders,
      body: JSON.stringify({ message: "Something went wrong" }),
    };
  }
};
