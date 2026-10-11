#include <stdio.h>

void printMainHelp(void){
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    CETWORKING - MAIN COMMANDS                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("📋 GENERAL COMMANDS:\n");
    printf("   /help                  - Display this help menu\n");
    printf("   /refresh               - Show connected users and available groups\n");
    printf("   /quit  or /q           - Disconnect from server and exit\n");
    printf("\n");
    
    printf("👥 USER & CHAT COMMANDS:\n");
    printf("   /chat <username>       - Start a 1-on-1 chat with a user\n");
    printf("                            Example: /chat Alice\n");
    printf("\n");
    printf("   /send                  - Send a file to the user you're chatting with\n");
    printf("                            (Available only inside an active chat)\n");
    printf("                            Example: /send\n");
    printf("\n");
    printf("   /back                  - Exit current chat and return to main menu\n");
    printf("                            (Available only inside an active chat)\n");
    printf("\n");
    
    printf("🏠 GROUP MANAGEMENT COMMANDS:\n");
    printf("   /create_group          - Create a new group (room/channel)\n");
    printf("                            Prompts for group name and description\n");
    printf("                            You become the admin of the group\n");
    printf("\n");
    printf("   /open <groupname>      - Join and open a group for chatting\n");
    printf("                            Example: /open MyGroup\n");
    printf("                            Type /back to exit the group chat\n");
    printf("\n");
    printf("   /add_member            - Add an existing user to a group\n");
    printf("                            Prompts for username and group name\n");
    printf("                            (Only group admin can add members)\n");
    printf("\n");
    printf("   /kick <username>       - Remove a member from the group\n");
    printf("                            Example: /kick Bob\n");
    printf("                            (Only group admin can kick members)\n");
    printf("                            (Only works inside an open group)\n");
    printf("\n");
    printf("   /leave_group <name>    - Leave a group permanently\n");
    printf("                            Example: /leave_group MyGroup\n");
    printf("\n");
    printf("   /info                  - Display group info and member list\n");
    printf("                            (Available only inside an active group)\n");
    printf("\n");
    
    printf("💡 TIPS:\n");
    printf("   - Use /refresh to see all online users and available groups\n");
    printf("   - You can only send files during 1-on-1 chats (use /send)\n");
    printf("   - Group admins have full control over members\n");
    printf("   - Type /help anytime to see this menu\n");
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n");
}

void printChatHelp(void){
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              CHAT MODE - AVAILABLE COMMANDS                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("📨 CHAT COMMANDS:\n");
    printf("   <message>              - Type any message to send to this user\n");
    printf("                            Just type and press Enter\n");
    printf("\n");
    printf("   /send                  - Send a file to the user\n");
    printf("                            You will be prompted for the filename\n");
    printf("\n");
    printf("   /back                  - Exit the chat and return to main menu\n");
    printf("\n");
    
    printf("💡 TIPS:\n");
    printf("   - Messages are sent directly to the user (peer-to-peer)\n");
    printf("   - Files are transferred through the server\n");
    printf("   - Type /back when done chatting\n");
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n");
}

void printGroupHelp(void){
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║            GROUP CHAT MODE - AVAILABLE COMMANDS                ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("💬 GROUP CHAT COMMANDS:\n");
    printf("   <message>              - Type a message to send to the group\n");
    printf("                            Everyone in the group will see it\n");
    printf("\n");
    printf("   /info                  - Show group information\n");
    printf("                            Displays group name, description, and members\n");
    printf("\n");
    printf("   /kick <username>       - Remove a member from the group (Admin only)\n");
    printf("                            Example: /kick Bob\n");
    printf("                            You must be the group admin\n");
    printf("\n");
    printf("   /back                  - Exit the group and return to main menu\n");
    printf("\n");
    
    printf("💡 TIPS:\n");
    printf("   - All messages in a group are visible to all members\n");
    printf("   - Only group admins can kick members\n");
    printf("   - Use /info to see who's in the group\n");
    printf("   - Type /back when done\n");
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n");
}

