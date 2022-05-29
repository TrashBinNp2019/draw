import express from 'express';
import { getUser, addUser, getFile, addFile, deleteUser } from '../controllers/users.js'
import { v4 as uuidV4 } from "uuid"
const router = express.Router();

router.get( '/:id', ( req, res ) => {
  res.send( getUser( req.params.id ) )

  console.log( "user " + req.params.id + " requested" );
} );

router.delete( '/:id', ( req, res ) => {
  res.send( deleteUser( req.params.id ) );

  console.log( "user " + req.params.id + " deleted" )
} ) ;

router.put( '/', ( req, res ) => {
  const ID = uuidV4();
  addUser( { id : ID, files : [] } )
  res.send( getUser( ID ) )

  console.log( "new user with id " + ID );
} );

router.get( '/:id/files/:name', ( req, res ) => {
  const file = getFile( req.params.id, req.params.name );
  if ( file.code !== 0 ) {
    res.status( 404 ).send();
  } else {
    res.sendFile( file.path )
  }

  console.log( "file " + req.params.name + " requested" );
} );

router.put( '/:id/files/:name', ( req, res ) => {
  if ( !req.files || Object.keys( req.files ).length === 0 ) {
      return res.status( 400 ).send( 'No files were uploaded.' );
  }
  const result = addFile( req.params.id, req.params.name, req.files.upload )
  if ( !result ) {
    return res.status( 400 ).send( 'Incorrect file name or user id' )
  }

  console.log( "file " + req.params.name + " uploaded" );
  return res.status( 200 ).send( 'Success' )
} );

export default router
