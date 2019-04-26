# obiew_front_end

#### Class Definition

store definition
```
store = {
    username: string,   // user login
    page: string,       // current page
    obiews: [obiew: object],    // feed of this user
}
```

obiew definition
```
obiew = {
    username: string, // owener of this obiew,
    obiew: string,    // content of this obiew,
    likes: [like: object],
    comments: [comment: object],
    timestamp: timestamp, // create timestamp,
}
```

like definition
```
like = {
    userId: int,
    obiewId: int,
}
```

comment definition
```
comment = {
    timestamp: timestamp, // create timestamp
    username: string, // who post the comment
    obiewId: int, // to which obiew it comments
    reply: string, // content of this comment
}
```

#### Workflow

##### Login
Post { username, password } to the server

if successfully login, convert to home page, get the feeds of this user

otherwise, convert to register page.

##### Register
Post { username, password, other infomation } to the server

if successfully registered, convert to login page

##### Post
Post an obiew of current user
```
Request: {
    obiew: string,
    username: string,
}

Response: {
    timestamp: timestamp, // create timestamp,
}
```

##### Comment
Post a comment of an obiew
```
Request: {
    username: string,
    obiewId: int,
    reply: string,
}
Response: {
    timestamp: timestamp, // create timestamp,
}
```

##### Like
Like an obiew of current user
```
Request: {
    username: string,
    obiewId: int,
}
```

##### Delete like
Delete a like of current user
```
Request: {
    username: string,
    obiewId: int,
}
```

#### Install all dependencies
```
npm install
```

#### Run the application
```
npm start
```

The application is now running at localhost:3000