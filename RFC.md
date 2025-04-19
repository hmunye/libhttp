RFC 9110:
-- 9. Methods

All general-purpose servers MUST support the methods GET and HEAD. All other
methods are OPTIONAL.

---

RFC 9112:
-- 2.2. Message Parsing

A sender MUST NOT send whitespace between the start-line and the first header 
field.

---

RFC 9112:
-- 3. Request Line

It is RECOMMENDED that all HTTP senders and recipients support, at a minimum,
request-line lengths of 8000 octets.

---

RFC 9112:
-- 3. Request Line

A server that receives a method longer than any that it implements SHOULD
respond with a 501 (Not Implemented) status code.

---

RFC 9112:
-- 3. Request Line

A server that receives a request-target longer than any URI it wishes to
parse MUST respond with a 414 (URI Too Long) status code.

---

RFC 9112:
-- 3.2. Request Target

Recipients of an invalid request-line SHOULD respond with either a 400 
(Bad Request) error or a 301 (Moved Permanently) redirect with the 
request-target properly encoded.

---

RFC 9112:
-- 3.2. Request Target

A server MUST respond with a 400 (Bad Request) status code to any
HTTP/1.1 request message that lacks a Host header field and to
any request message that contains more than one Host header field
line or a Host header field with an invalid field value.

---

RFC 9112:
-- 5.1. Field Line Parsing

A server MUST reject, with a response status code of 400 (Bad Request), any 
received request message that contains whitespace between a header field name 
and colon.
