import { serve } from '@hono/node-server'
import { Hono } from 'hono'

const app = new Hono()
const users: Record<string, string> = {
  "admin": "admin",
  "ayaan": "lol123"
}

app.get('/', (c) => {
  console.log("added request")
  return c.text('Hello Hono!')
})

app.get('/name', (c) => {
  console.log("added request")
  return c.json({
    name: "Application"
  })
})

app.post('/signup', async (c) => {
  const body = await c.req.json() as  {username: string, password: string}
  console.log(body)
  if (Object.keys(users).includes(body.username)) {
    return c.json({
      success: false,
      errorcode: 1,
      msg: "Account already exists!"
    })
  }
  users[body.username] = body.password
  return c.json({
    success: true,
    errorcode: 0,
    msg: "Account created"
  })
})

app.post('/signin', async (c) => {
  const body = await c.req.json() as  {username: string, password: string}
  console.log(body)
  if (!Object.keys(users).includes(body.username)) {
    return c.json({
      success: false,
      errorcode: 1,
      msg: `${body.username} is not a registered account`
    })
  }
  if (users[body.username] != body.password) {
    return c.json({
      success: false,
      errorcode: 2,
      msg: `Incorrect password for ${body.username}`
    })
  }
  return c.json({
    success: true,
    errorcode: 0,
    msg: "Signed-in successfully!"
  })
})

serve({
  fetch: app.fetch,
  port: 3000,
  hostname: "0.0.0.0"
}, (info) => {
  console.log(`Server is running on http://localhost:${info.port}`)
})
