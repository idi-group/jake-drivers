package com.ugla.jake;

public class JAKEPackets {
	
	public static final int JAKE_BAD_PACKET = -1;
	
	public static final int JAKE_NUM_PACKET_TYPES = 7;
	
	public static final int JAKE_HEADER_LEN = 4;

	// packet types
	public static final int JAKE_DATA = 0;					
	public static final int JAKE_CMD_READ = 1;
	public static final int JAKE_CMD_BT_READ = 2;
	public static final int JAKE_CMD_WRITE = 3;
	public static final int JAKE_CMD_BT_WRITE = 4;
	public static final int JAKE_ACK_ACK = 5;
	public static final int JAKE_ACK_NEG = 6;
	
	public static final int JAKE_READ_OK = 1;
	public static final int JAKE_READ_ERROR	= -1;
	public static final int JAKE_READ_CONTINUE =	2;

	
	public static int[] jake_packet_types = { 
		0, 1, 2, 3, 4, 5, 6,
	};
	
	public static byte[][] jake_byte_packet_headers = new byte[7][4];
	private static boolean headers_initialised = false;
	
	public static void initialiseHeaders() {
		if(headers_initialised)
			return;
		
		for(int i=0;i<jake_packet_headers.length;i++) {
			jake_byte_packet_headers[i] = jake_packet_headers[i].getBytes();
		}
		headers_initialised = true;
	}

	public static String jake_packet_headers[] = {
		"$$$D",		// data
		
		"$$R", 		// command; read main microcontroller
		"$$r", 		// command; read BT microcontroller
		
		"$$W", 		// command; write main microcontroller
		"$$w", 		// command; write BT microcontroller
		
		"$$$A", 	// ack; positive
		"$$$N", 	// ack; negative
	};

	public static int jake_packet_lengths[] = {
		24,
		
		6,
		6,
		
		6,
		6,
		
		24,
		24,
	};

	public static int classifyHeader(byte[] b, int len) {
		int type = JAKE_BAD_PACKET;
		
		if(len != JAKE_HEADER_LEN)
			return type;
		
		int i = 0;
		for(;i<jake_packet_types.length;i++) {
			if(JAKEUtils.memcmp(jake_byte_packet_headers[i], b, 0, JAKE_HEADER_LEN))
				return i;
		}
		
		return type;
	}

	public static int fixBytes(int b1, int b2) {
		int res = (int)((b1 & 0x00ff) + (((b2 & 0xff) << 8)));
		if((res & 0x8000) != 0)
			res |= ~0x7FFF;
		return res;
	}

}
