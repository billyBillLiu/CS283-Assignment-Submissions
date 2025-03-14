1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

```
The client knows it's done receiving output when it detects a special end-of-message character (like RDSH_EOF_CHAR) sent by the server. Since recv() might not get all the data at once, the client has to loop, calling recv() multiple times until it gets everything. Some ways to handle this include using a fixed message size, adding a length header to tell the client how much data to expect, or using delimiters like newline (\n) or null (\0). Without these, the client might stop too early or read extra unintended data.
```
2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

```
Since TCP just gives us a stream of bytes with no built-in message separation, we need to define our own way to mark message boundaries. A common method is to add a special delimiter (like \n or \0), or prefix each message with its length so the receiver knows exactly how many bytes to read. If we don’t handle this properly, the client might get half a message and think it's complete, or combine multiple messages into one, which would mess up command execution.
```

3. Describe the general differences between stateful and stateless protocols.

```
A stateful protocol keeps track of what’s happening between requests, meaning the server remembers things like sessions or previous interactions (e.g., TCP, FTP, SSH). A stateless protocol, on the other hand, treats each request as independent, with no memory of past interactions (e.g., UDP, HTTP). Stateful protocols are useful when you need things like authentication or persistent connections, but stateless ones are better for scalability since the server doesn’t have to remember anything.
```

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

```
Even though UDP doesn’t guarantee message delivery, ordering, or error correction, it’s still useful in situations where speed is more important than reliability. For example, in online gaming, video streaming, or VoIP calls, we’d rather have a small packet drop occasionally than wait for TCP’s retransmission, which could cause lag. UDP is also great for broadcasting messages to multiple devices at once, like in DNS or live streaming.
```

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

```
The OS provides the sockets API, which gives us functions to create network connections. With sockets, we can open a connection using socket(), connect to another machine with connect(), send data with send(), and receive data with recv(). There are different types of sockets depending on whether we want reliable (TCP) or fast (UDP) communication. Sockets make it possible for applications to send messages over a network without dealing with the low-level details of how data gets there.
```