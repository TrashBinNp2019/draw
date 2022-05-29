import fs from 'fs';
import path from 'path';

let users = [
  {
    id: "1",
    files: [
      { name: "hello3.svg" },
      { name: "new.svg" },
    ]
  }
];

export const getUser = function( _id ) {
  return users.find( user => { return user.id === _id } );
}

export const addUser = function( user ) {
  users.push( user )
}

export const deleteUser = function( _id ) {
  const len = users.length
  users = users.filter( user => { return user.id !== _id } );
  return !( len === users.length )
}

export const getFile = function( _id, _name ) {
  const _path = path.join( path.resolve(), `/files/${ _id }/${ _name }` )
  if ( !fs.existsSync( _path ) ) return { code: 1 }
  else return { code: 0, path: _path }
}

export const addFile = function( _id, _name, _file ) {
  let success = true

  const _path = path.join( path.resolve(), `/files/${ _id }/${ _name }` )
  if ( !fs.existsSync( path.dirname( _path ) ) )
    fs.mkdirSync( path.dirname( _path ) )

  _file.mv( _path, err => {
    if ( err ) success = false
  } );
  if ( !success ) return false
  success = false

  users.map( user => {
    if ( user.id !== _id ) return

    let contains = user.files.filter( file => file.name === _name ).length > 0
    if ( !contains ) user.files = [ ...user.files, { name:_name } ]
    success = true
  } )
  return success
}
