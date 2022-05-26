import fs from 'fs';
import path from 'path';

let users = [
  {
    id: "1",
    files: [
      { name: "hello.jpg" }
    ]
  }
];

export const getUser = function( _id ) {
  return users.find( user => { return user.id === _id } );
}

export const addUser = function( user ) {
  users.push( user )
}

export const getFile = function( _id, _name ) {
  const _path = path.join( path.resolve(), `/files/${ _id }/${ _name }` )
  if ( !fs.existsSync( _path ) ) return { code: 1 }
  else return { code: 0, path: _path }
}

export const addFile = function( _id, _name, _contents ) {

}
