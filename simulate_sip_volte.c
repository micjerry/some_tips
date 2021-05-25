#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#define PORT 5060
#define MAXLINE 2048

char test_sip[2048] = "REGISTER sip:ims.mnc092.mcc466.3gppnetwork.org SIP/2.0\r\n"
					  "To: <sip:460029999999032@ims.mnc002.mcc460.3gppnetwork.org>\r\n"
					  "From: <sip:460029999999032@ims.mnc002.mcc460.3gppnetwork.org>;tag=bfecb-W\r\n"
					  "Contact: <sip:460029999999032@182.120.12.64:5060>;"
					  "+sip.instance=\"<urn:gsma:imei:86704904-713828-0>\";"
					  "+g.3gpp.accesstype=\"cellular2\";"
					  "audio;video;+g.3gpp.smsip;"
					  "+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\"\r\n"
					  "Expires: 600000\r\n"
					  "P-Access-Network-Info: 3GPP-NR-TDD;utran-cell-id-3gpp=4600200005100001C001\r\n"
					  "Supported: path,sec-agree\r\n"
					  "Allow: INVITE,ACK,OPTIONS,BYE,CANCEL,UPDATE,PRACK,NOTIFY,MESSAGE,REFER\r\n"
					  "Require: sec-agree\r\n"
					  "Proxy-Require: sec-agree\r\n"
					  "Call-ID: aeecb-Wwo@182.120.12.64\r\n"
					  "CSeq: 1 REGISTER\r\n"
					  "Max-Forwards: 70\r\n"
					  "Via: SIP/2.0/UDP 182.120.12.64:5060;branch=z9hG4bKcgecb-WwoWjWTpaaa4wY;rport\r\n"
					  "User-Agent: IM-client/OMA1.0 HW-Rto/V1.0\r\n"
					  "Content-Length: 0\r\n"
					  ;
					  

#define FIX_TAG ";tag=674d4815-5ee27d7a-90-7f7c7894c6f8-3478580a-13c4-7217"
#define HEADER_NAME_CALLID "Call-ID"
#define HEADER_NAME_FROM "From"
#define HEADER_NAME_TO "To"
#define HEADER_NAME_CSEQ "CSeq"
#define HEADER_NAME_CONTACT "Contact"
#define HEADER_NAME_VIA "Via"
#define HEADER_NAME_CL "Content-Length"
#define HEADER_NAME_EXPIRE "Expires"

struct sip_requestline {
	char *name;
	char *request;
	char *version;
};

struct sip_responseline {
	char *version;
	char *code;
	char *desc;
};

struct sip_line {
	char *name;
	char *value;
};

struct sip_request {
	struct sip_requestline *reqline;
	struct sip_line *headers[32];
	int header_num;
};

struct sip_response {
	struct sip_responseline *resline;
	struct sip_line *headers[32];
	int header_num;
};

struct sip_responseline ok = {"SIP/2.0", "200", "OK"};

void parse_sip(char *raw, struct sip_request **sip_reg);
void parse_request(char *line, struct sip_requestline **result);
void parse_line(char *line, struct sip_line **result);
void make_response(struct sip_request *request, struct sip_response **response);
struct sip_line *find_header(struct sip_request *request, char *name);

void make_toheader(struct sip_line *reqline, struct sip_line **resline);
void make_viaheader(struct sip_line *reqline, struct sip_line **resline);
void make_contactheader(struct sip_line *contactline, struct sip_line *expireline, struct sip_line **resline);
void make_pai(struct sip_requestline *line, struct sip_line **resline);

void gen_send(struct sip_response *response, char **result, int *rlen);

int main()
{   
    char buffer[MAXLINE];
    char *message = "Sip Server";
    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));
	struct sip_request *sip_reg = NULL;
	struct sip_response *response = NULL;
	char *send_buffer = NULL;
	int send_len = 0;
	
	parse_sip(test_sip, &sip_reg);
	
	if (!sip_reg) {
		printf("Can not parse sip message\n");
		return 0;
	}
	
	make_response(sip_reg, &response);
	
	if (!response) {
		printf("Can not gen sip response\n");
	}
	
	gen_send(response, &send_buffer, &send_len);
	
	if (send_buffer) {
		printf("result: %s\n", send_buffer);
	}
	
	return 0;
  
    // Create a UDP Socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);        
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET; 
   
    // bind server address to socket descriptor
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
       
    //receive the datagram
    len = sizeof(cliaddr);
    int n = recvfrom(listenfd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&cliaddr,&len); //receive message from client
    buffer[n] = '\0';
	parse_sip(buffer, &sip_reg);
	
	if (!sip_reg) {
		printf("Can not parse sip message\n");
		return 0;
	}
	
	make_response(sip_reg, &response);
	
	if (!response) {
		printf("Can not gen sip response\n");
		return 0;
	}
	
	gen_send(response, &send_buffer, &send_len);
           
    // send the response
    sendto(listenfd, send_buffer, send_len, 0,
          (struct sockaddr*)&cliaddr, sizeof(cliaddr));
		  
	return 0;
}

void parse_sip(char *raw, struct sip_request **sip_reg)
{
	char *line = NULL;
	const char s[2] = "\r\n";
	struct sip_request *sip_req;
	int i = 0;
	
	sip_req = malloc(sizeof(*sip_req));
	memset(sip_req, 0, sizeof(struct sip_request));
	
	line = strtok(raw, s);
	printf( " %s\n", line);
	parse_request(line, &sip_req->reqline);
	while (line != NULL) {
		line = strtok(NULL, s);
		if (line) {
			printf( " %s\n", line);
			parse_line(line, &sip_req->headers[i++]);
		}
	}
	
	sip_req->header_num = i;
	
	*sip_reg = sip_req;
}

void parse_line(char *line, struct sip_line **result)
{
	char *pos = line;
	char name[64];
	char value[256];
	int i = 0;
	struct sip_line *p_line;
	
	memset(name, 0, sizeof(name));
	memset(value, 0, sizeof(value));
	
	p_line = malloc(sizeof(*p_line));
	
	while ((name[i++] = *pos++) != ':') {
	}
	name[i-1] = '\0';
	
	p_line->name = strdup(name);
	
	i = 0;
	while (*pos == ' ') {
		pos++;
	}
	
	while ((value[i++] = *pos++) != '\0') {
	}
	
	p_line->value = strdup(value);
	
	*result = p_line;
	
	printf( "header: %s\n", p_line->name);
	printf( "value: %s\n", p_line->value);
}

void parse_request(char *line, struct sip_requestline **result)
{
	char *pos = line;
	char name[32];
	char request[256];
	char version[32];
	int i = 0;
	struct sip_requestline *p_line;
	
	while ((name[i++] = *pos++) != ' ') {
	}
	name[i-1] = '\0';
	
	i = 0;
	while (*pos == ' ') {
		pos++;
	}
	
	while ((request[i++] = *pos++) != ' ') {
	}
	request[i-1] = '\0';
	
	i = 0;
	while (*pos == ' ') {
		pos++;
	}
	
	while ((version[i++] = *pos++) != '\0') {
	}
	
	p_line = malloc(sizeof(*p_line));
	
	p_line->name = strdup(name);
	p_line->request = strdup(request);
	p_line->version = strdup(version);
	
	*result = p_line;
	
	printf( "name: %s\n", p_line->name);
	printf( "request: %s\n", p_line->request);
	printf( "version: %s\n", p_line->version);
}

void make_response(struct sip_request *request, struct sip_response **response)
{
	struct sip_response *p_response = NULL;
	struct sip_line *line;
	struct sip_line *expire_line;
	struct sip_line *res_line = NULL;
	int i = 0;
	
	*response = NULL;
	
	p_response = malloc(sizeof(*p_response));
	memset(p_response, 0, sizeof(struct sip_response));
	
	//request line
	p_response->resline = &ok;
	
	//from
	line = find_header(request, HEADER_NAME_FROM);
	if (!line)
		return;
	p_response->headers[i++] = line;
	
	//to
	line = find_header(request, HEADER_NAME_TO);
	if (!line)
		return;
		
	make_toheader(line, &res_line);
	p_response->headers[i++] = res_line;
	
	//Call-ID
	line = find_header(request, HEADER_NAME_CALLID);
	if (!line)
		return;
		
	p_response->headers[i++] = line;
	
	//Cseq
	line = find_header(request, HEADER_NAME_CSEQ);
	if (!line)
		return;
		
	p_response->headers[i++] = line;
	
	//Via
	line = find_header(request, HEADER_NAME_VIA);
	make_viaheader(line, &res_line);
	p_response->headers[i++] = res_line;
	
	//Contact
	expire_line = find_header(request, HEADER_NAME_EXPIRE);
	line = find_header(request, HEADER_NAME_CONTACT);
	if (!line || !expire_line)
		return;
		
	make_contactheader(line, expire_line, &res_line);
		
	p_response->headers[i++] = res_line;
	
	//P-Associated-URI
	make_pai(request->reqline, &res_line);
	p_response->headers[i++] = res_line;
	
	//Content-Length
	line = find_header(request, HEADER_NAME_CL);
	if (!line)
		return;
		
	p_response->headers[i++] = line;
	
	p_response->header_num = i;
	
	*response = p_response;
}

struct sip_line *find_header(struct sip_request *request, char *name)
{
	int i;
	struct sip_line *line;
	
	if (request == NULL)
		return NULL;
		
	for (i = 0; i < request->header_num; i++) {
		line = request->headers[i];
		if (!line)
			return NULL;
			
		if (strcmp(line->name, name) == 0) {
			return line;
		}
	}
	
	return NULL;
}

void make_toheader(struct sip_line *reqline, struct sip_line **resline)
{
	struct sip_line *p_line = NULL;
	int len;
	
	p_line = malloc(sizeof(*p_line));
	p_line->name = strdup(reqline->name);
	len = strlen(reqline->value) + strlen(FIX_TAG) + 1;
	p_line->value = malloc(len);
	memset(p_line->value, '\0', len);
	strcpy(p_line->value, reqline->value);
	strcat(p_line->value, FIX_TAG);
	
	*resline = p_line;
}

void make_contactheader(struct sip_line *contactline, struct sip_line *expireline, struct sip_line **resline)
{
	struct sip_line *p_line = NULL;
	int len;
	int i = 0;
	char *contact_pos = contactline->value;
	char res_contact[512];
	char expire[64];
	
	memset(res_contact, 0, sizeof(res_contact));
	memset(expire, 0, sizeof(expire));
	
	p_line = malloc(sizeof(*p_line));
	
	p_line->name = strdup(contactline->name);
	
	while ((res_contact[i++] = *contact_pos++) != ';') {
	}
	
	sprintf(expire, "expires=%s;", expireline->value);
	strcat(res_contact, expire);
	i += strlen(expire);
	contact_pos++;
	while ((res_contact[i++] = *contact_pos++) != '\0') {
	}
	
	p_line->value = strdup(res_contact);
	*resline = p_line;
}

void make_viaheader(struct sip_line *reqline, struct sip_line **resline)
{
	struct sip_line *p_line = NULL;
	char via[256];
	int i = 0;
	char *pos = reqline->value;
	
	p_line = malloc(sizeof(*p_line));
	p_line->name = strdup(reqline->name);
	
	while ((via[i++] = *pos++) != ';') {
	}
	
	if (strncmp(pos, "rport", 5) == 0) {
		via[i++] = 'r';
		via[i++] = 'p';
		via[i++] = 'o';
		via[i++] = 'r';
		via[i++] = 't';
		via[i++] = '=';
		via[i++] = '5';
		via[i++] = '0';
		via[i++] = '6';
		via[i++] = '0';
		via[i++] = ';';
		while (*pos++ != ';') {
		}
		
		while ((via[i++] = *pos++) != '\0') {
		}
	} else {
		while ((via[i++] = *pos++) != ';') {
		}
		
		if (strncmp(pos, "rport", 5) == 0) {
			via[i++] = 'r';
			via[i++] = 'p';
			via[i++] = 'o';
			via[i++] = 'r';
			via[i++] = 't';
			via[i++] = '=';
			via[i++] = '5';
			via[i++] = '0';
			via[i++] = '6';
			via[i++] = '0';
			via[i++] = ';';
			via[i++] = '\0';
		}
	}
	
	p_line->value = strdup(via);
	
	*resline = p_line;
}

void make_pai(struct sip_requestline *line, struct sip_line **resline)
{
	char domain[128];
	char value[256];
	char *pos = line->request;
	int i;
	struct sip_line *p_line = NULL;
	
	while (*pos++ != ':') {
	}
	pos++;
	while ((domain[i++] = *pos++) != '\0') {
	}
	
	p_line = malloc(sizeof(*p_line));
	p_line->name = "P-Associated-URI";
	sprintf(value, "<sip:9032@%s>,<tel:9032>", domain);
	p_line->value = strdup(value);
	
	*resline = p_line;
}

void gen_send(struct sip_response *response, char **result, int *rlen)
{
	struct sip_responseline *res_line = response->resline;
	struct sip_line *line = NULL;
	int len, i, total;
	char *buffer;
	char *pos;
	
	total = 0;
	buffer = malloc(MAXLINE);
	pos = buffer;
	memset(buffer, '\0', MAXLINE);
	
	//response line
	len = strlen(res_line->version);
	memcpy(pos, res_line->version, len);
	pos += len;
	total += len;
	
	*pos++ = ' ';
	total += 1;
	len = strlen(res_line->code);
	memcpy(pos, res_line->code, len);
	pos += len;
	total += len;
	*pos++ = ' ';
	total += 1;
	
	len = strlen(res_line->desc);
	memcpy(pos, res_line->desc, len);
	pos += len;
	total += len;
	*pos++ = '\r';
	total += 1;
	*pos++ = '\n';
	total += 1;
	
	//response headers
	for (i = 0; i < response->header_num; i++) {
		line = response->headers[i];
		if (!line)
			break;
		
		len = strlen(line->name);
		memcpy(pos, line->name, len);
		pos += len;
		total += len;
		
		*pos++ = ':';
		total += 1;
		*pos++ = ' ';
		total += 1;
		
		len = strlen(line->value);
		memcpy(pos, line->value, len);
		pos += len;
		total += len;
		*pos++ = '\r';
		total += 1;
		*pos++ = '\n';
		total += 1;
	}
	
	*result = buffer;
	*rlen = total;
}
