import express from 'express';
import bodyParser from 'body-parser';
import router from './routes/files.js';

const app = express();

app.use( bodyParser.json() )

app.use( '/api', router );

app.listen( 3000, 'localhost', err => {
  if( !err ) {
    console.log( 'Server running' );
  } else {
    console.log( err.message );
  }
} );
