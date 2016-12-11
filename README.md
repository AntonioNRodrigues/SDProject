# SDProject

Sistemas Distribuidos 2016/2017

<h3>Grupo 33 </h3>
<ul>
<li>Miguel Vale n.º39279  </li> 
<li>António Rodrigues n.º40853  </li>
<li>Ricardo Veloso n.º44842  </li>
</ul>
<h4>4.ºPhase<h4>
<p></p>
<h5>Build Project</h5>
<ul>
<li><b>make</b> build all project</li>
<p><b>Obs:</b> This only works if all the files of test [test_data.c, test_entry.c, test_list.c, test_message.c, test_table.c] are present.</p>
<li><b>make table-client</b> build the client</li>
<li><b>make table-server</b> build the server</li>
  
</ul>
<h5>Run the clients and the server</h5>
<ul>
<li><b>Run Secundary Server</b> ./table-server tcp_port size_table </li>
<li><b>Alternative</b> ./secundary.sh </li> 
<li><b>Run Primary Server</b> ./table-server tcp_port size_table ip_secundary:port_secundary</li>
<li><b>Alternative</b> ./primary.sh [<b>PS:</b> default ip is 127.0.0.1] </li>
<li><b>Run Client</b> ./table-client ip_primary:port_primary ip_secundary:port_secundary</li>
<li><b>Alternative</b> ./client.sh [<b>PS:</b> default ip is 127.0.0.1]</li>
<li><b><u>The port for the primary server is include in the code and its 44444</u></b></li>
</ul>

<p>-----------------------------------------------------------------------------------------------------------------------<p>

<h4>3.ºPhase<h4>
<p>In this version the server accepts multiple clients</p>
<h5>Build Project</h5>
<ul>
<li><b>make</b> build all project</li>
<p><b>Obs:</b> This only works if all the files of test [test_data.c, test_entry.c, test_list.c, test_message.c, test_table.c] are present.</p>
<li><b>make table-client</b> build the client</li>
<li><b>make table-server</b> build the server</li>
  
</ul>
<h5>Run the clients and the server</h5>
<ul>
<li><b>Run Server</b> ./table-server tcp_port size_table</li>
<li><b>Run Client</b> ./table-client ip_server:port_server</li>
</ul>

<p>-----------------------------------------------------------------------------------------------------------------------<p>

<h4>2.ºPhase<h4>
<h5>Build Project</h5>
<ul>
<li><b>make</b> build all project</li>
<p><b>Obs:</b> This only works if all the files of test [test_data.c, test_entry.c, test_list.c, test_message.c, test_table.c] are present.</p>
  <li><b>make teste_table</b> build the test for the table</li>
  <li><b>make teste_message</b> build the test of message</li>
  <li><b>make table-client</b> build the client</li>
  <li><b>make table-server</b> build the server</li>
  
</ul>
<h5>Run the client and the server</h5>
<ul>
<li><b>Run Server</b> ./table-server tcp_port size_table</li>
<li><b>Run Client</b> ./table-client ip_server:port_server</li>
</ul>

<p>-----------------------------------------------------------------------------------------------------------------------<p>

<h4>1.ºPhase<h4>
<h5>Build Project</h5>
<ul>
<li><b>make</b> build all project</li>
<p><b>Obs:</b> This only works if all the files of test [test_data.c, test_entry.c, test_list.c] are present.</p>
  <li><b>make teste_data</b> build the test for the data</li>
  <li><b>make teste_entry</b> build the test for the entry</li>
  <li><b>make table_list</b> build the test for the list</li>
</ul>
