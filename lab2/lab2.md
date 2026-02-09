# COL331 Lab 2: Undo Logs in xv6
In the `p12-log` branch, you have studied the implementation of write-ahead
logging (WAL) with redo logs in `xv6`, which helped ensure atomicity with
respect to crashes, and hence crash consistency.

In this lab, you will explore another method of write ahead logging, which uses
_undo logs_ instead of the redo logs. 

## Undo Logs
The idea of undo logs is as follows: before we write a block to the disk, we
first log the *old value* of the block, then write new blocks to the disk, and
finally commit the transaction. In case of a crash, we simply check the log for
uncommitted transactions, and "undo" their effect. 

We can describe this procedure more formally using the following rules:

### Undo Log Rules 
Say we are writing new values `n1`, `n2` to blocks B1, B2:

1. Write old values `o1`, `o2` that were in blocks B1, B2 into the log
2. Start the transaction: write the log header block containing the block numbers of B1 and B2
3. Install the transaction: flush the values `n1` and `n2` to B1, B2 on disk 
4. Commit the transaction by clearing the log

### Undo Log Recovery

Read the log header block and write back the blocks in the log as described in
the log header.

We may summarize the difference between redo and undo logging via the following
table: 

| Status of block at time of crash | Redo Log                                     | Undo Log                                     |
| -------------------------------- | -------------------------------------------- | -------------------------------------------- |
| Committed & written to disk      | Nothing to do                                | Nothing to do                                |
| Committed but not written        | Redo by writing the new value in log to disk | Not possible                                 |
| Written but not committed        | Not possible                                 | Undo by writing the old value in log to disk |
| Not written and not committed    | Nothing to do                                | Nothing to do                                |

### Food for Thought
Try to compare performance and durability of redo and undo logs. Which method of
logging would be more efficient? And which method might lose more data if we
assume rest of the system design is identical. 

## Implementation
The implementation of undo logging will involve changes to `log.c` and `bio.c`.
You are encouraged to read and understand `p12-log.md`, as well as how the
buffer cache works. 

Note that in undo logs, we write the old value of a block to the log which helps
us undo the operation, which may require an additional read from the disk.

To prevent this performance hit, we cache the old value of disk whenever we
perform a write. For this purpose, a new function `bread_wr` has been defined in
`bio.c`, which is now called in `fs.c` wherever `bread` was called with the
intention of *modifying the block*. You must provide an implementation of
`bread_wr` which efficiently caches the old value in the disk, which can be
later used when writing to the log. 

Finally, note that in redo logs, we write to the log lazily, i.e. the
`write_log` function is called only during commit. However, instead of waiting
till `commit`, we can now eagerly write to the log whenever a block is changed.
Your task is to modify the code to make writes to the undo log in an eager
manner.

Note that you are strictly **not allowed** to make any changes to `main.c`, the
`commit` function in `log.c`, and to any of the calls to `bread_wr` in `fs.c`.
Apart from this, you are allowed to create new functions, modify existing ones,
add attributes to structs, etc.

To test your implementation, refer to [testing.md](./testing.md).

## Deliverables
In summary, you are required to:
- Replace the logging system in xv6 with undo logging
- Ensure your implementation does not entail extra reads from disk
- Make sure writes to logs are eager

You will be required to submit the entire xv6 folder with your changes. 

In the xv6 root directory, run the following commands:

```
make clean
tar czvf lab2_<entryNumber>.tar.gz *
```
 
This will create a tarball with the name, lab2_<entryNumber>.tar.gz in the same
directory. Submit this tarball on Moodle. Entry number format: 2020CS10567. (All
English letters will be in capitals in the entry number)
