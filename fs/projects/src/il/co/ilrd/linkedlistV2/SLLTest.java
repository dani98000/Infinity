package il.co.ilrd.linkedlistV2;
import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Tag;
import org.junit.jupiter.api.Test;

public class SLLTest {
	
	@Test
	@DisplayName("😱")
	@Tag("fast")
	public void test1() {
		int res = 0;
		SinglyLinkedList list = new SinglyLinkedList();

		//test1
		list.pushFront(1);
		list.pushFront(2);
		list.pushFront(3);
		
		res = list.getSize();
		assertEquals(res, 3);
		
		//test2
		list.popBack();
		res = list.getSize();
		assertEquals(res, 2);
	
		//test3
		res = (Integer)(list.begin().next());
		assertEquals(res, 3);
		
		//test4
		assertEquals(list.isEmpty(), false);
	}
	
	@Test
	public void test2() {
		int res = 0;
		SinglyLinkedList list = new SinglyLinkedList();
		Iterator current = list.begin();
		
		for(int i = 0 ; i < 10 ; ++i) {
			list.pushFront(i);
		}
		
		res = list.getSize();
		assertEquals(res, 10);
		
		int i = 0;
		while(current.hasNext()) {
			assertNotEquals(list.find(i), i);
			++i;
			current.next();
		}
		
		assertEquals(res, 10);

	}
}
