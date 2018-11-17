# time and delay

### `proc_create_data`
```
	proc_create_data("currentime", 0, NULL, &jit_currentime_fops, NULL);
```
create a entry in the proc directory


### `wait`
```
wait_queue_head_t wait;
```
https://stackoverflow.com/questions/19942702/the-difference-between-wait-queue-head-and-wait-queue-in-linux-kernel
