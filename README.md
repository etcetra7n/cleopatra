# Xthreads Backend functions

This is the drectory for the a netlify function used for the Xthreads application. The URL to access this function is https://xthreads.netlify.app/.netlify/functions/generate-thread?topic={topic}

It uses the Gemini Node.js API to generate the thread in a JSON format which can be easily parsed in the front end

### Our prompt to Gemini

"Generate a twitter thread for the topic "${topic}". This is for educational purpose. Include facts and other interesting things to read. Keep it straightforward and don't use any emojis. It should sound like a informative article. The thread should have 5 tweets including catchy starting tweet. Give result in a JSON format"

### Why a backend function is necasssary

Backend function is necassary because if I were to use the Gemini API in the front end, that would mean exposing my gemini API key to the public, which can lead to misuse
