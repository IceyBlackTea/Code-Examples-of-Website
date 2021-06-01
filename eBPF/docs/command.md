# Linux Command

### `nc`

#### check usage

```
man nc
```

#### listen from port with udp temporarily

```
nc -lu -p [source_port]
```

#### listen from port with tcp temporarily

```
nc -l -p [source_port]
```

#### send messages with udp

```
nc -u [-p [source_port]] [destination] [port]
```

#### send messages with tcp

```
nc [-p [source_port]] [destination] [port]
```
