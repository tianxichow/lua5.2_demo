function user_type(a,x)
  npc1 = npclib.new(10001,"range")
  npc2 = npclib.new(10002,"chow")
  print("npc1 id = " .. npc1:get_id())
  npc2:set_name("rangechow")
  print("npc2 name = " .. npc2:get_name())
  print(npc1)
end

function thread_yield(req_id)
  print("req_id:"..tostring(req_id).." start")
  y, z= netiolib.send_and_recv(req_id)
  print("req_id:"..tostring(req_id).." recv y="..tostring(y).." z="..tostring(z))
  return y + z
end

function c_lua(x)
  myclib.extra(x);
  return 1
end