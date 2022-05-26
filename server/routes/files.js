import express from 'express';
import { getUser, addUser, getFile, addFile } from '../controllers/users.js'
import { v4 as uuidV4 } from "uuid"
const router = express.Router();

router.get( '/:id', ( req, res ) => {
  res.send( getUser( req.params.id ) )
} );

router.put( '/', ( req, res ) => {
  const _files = req.body.files
  const ID = uuidV4();
  addUser( { id : ID, files : _files } )
  res.send( getUser( ID ) )
} );

router.get( '/:id/files/:name', ( req, res ) => {
  const file = getFile( req.params.id, req.params.name );
  if ( file.code !== 0 ) {
    res.status( 404 ).send();
  } else {
    res.sendFile( file.path )
  }
} );

router.put( '/:id/files/:name', ( req, res ) => {
  res.send( addFile( req.params.id, req.params.name ) );
} );

export default router
